#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <string.h>

#include "config.h"
#include "sensor_interface.h"
#include "fusion.h"

static WiFiClient espClient;
static PubSubClient mqtt(espClient);

static unsigned long last_publish_ms = 0;

// ================= WiFi =================
static void connectWiFi()
{
  Serial.printf("[WiFi] Connecting to %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (millis() - start > 20000)
    {
      Serial.println("\n[WiFi] Timeout, retrying...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      start = millis();
    }
  }
  Serial.printf("\n[WiFi] Connected. IP: %s\n",
                WiFi.localIP().toString().c_str());
}

// ================= MQTT =================
static void connectMQTT()
{
  while (!mqtt.connected())
  {
    Serial.print("[MQTT] Connecting... ");
    String clientId = String("xiao-master-") +
                      String((uint32_t)esp_random(), HEX);
    if (mqtt.connect(clientId.c_str()))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.printf("failed rc=%d, retrying in 2s\n", mqtt.state());
      delay(2000);
    }
  }
}

// ================= Time =================
static String isoTimestamp()
{
  time_t now = time(nullptr);
  if (now < 100000)
    return "1970-01-01T00:00:00Z";

  struct tm tmv;
  gmtime_r(&now, &tmv);
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tmv);
  return String(buf);
}

// ================= Publish one node =================
static void publishNode(const NodeData &n, const String &ts)
{
  JsonDocument doc;
  doc["node_id"] = n.node_id;
  doc["timestamp"] = ts;
  doc["latitude"] = n.latitude;
  doc["longitude"] = n.longitude;
  doc["gps_valid"] = n.valid;
  doc["t1"] = n.sensors.t1;
  doc["t2"] = n.sensors.t2;
  doc["temperature"] = n.fused;
  doc["rssi"] = WiFi.RSSI();
  doc["source"] = n.is_master ? "self" : "lora";

  char payload[512];
  size_t len = serializeJson(doc, payload, sizeof(payload));

  String topic = String(MQTT_TOPIC_PREFIX) + n.node_id + "/data";

  bool ok = mqtt.publish(topic.c_str(), payload, len);
  Serial.printf("[PUB] %-28s %s\n", topic.c_str(), ok ? "ok" : "FAIL");
}

// ================= Build all node data =================
static size_t buildNodeData(NodeData *out, size_t max_count)
{
  size_t n = 0;

  // ----- Master's own entry (index 0) -----
  if (n < max_count)
  {
    NodeData &m = out[n];
    strncpy(m.node_id, NODE_DEFS[MASTER_NODE_INDEX].node_id,
            sizeof(m.node_id) - 1);
    m.node_id[sizeof(m.node_id) - 1] = '\0';

    GpsReading g = master_gps_read();
    m.latitude = g.latitude;
    m.longitude = g.longitude;
    m.valid = g.valid;
    m.sensors = master_sensors_read();
    m.is_master = true;

    fusion_set_slot(0);
    m.fused = fuse_temperature(m.sensors);
    ++n;
  }

  // ----- Remote LoRa nodes (index 1..N) -----
  size_t remotes = remote_nodes_read(out + n, max_count - n);
  for (size_t i = 0; i < remotes; ++i)
  {
    fusion_set_slot((int)(n + i));
    out[n + i].fused = fuse_temperature(out[n + i].sensors);
  }
  n += remotes;

  return n;
}

// ================= Arduino entry points =================
void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== EE2120 Master Gateway (XIAO ESP32S3) ===");
  Serial.printf("Master ID   : %s\n", NODE_DEFS[0].node_id);
  Serial.printf("Remote nodes: %u\n", (unsigned)REMOTE_NODE_COUNT);
  Serial.printf("Dummy modes : sensors=%d gps=%d lora=%d\n",
                (int)USE_DUMMY_SENSORS,
                (int)USE_DUMMY_GPS,
                (int)USE_DUMMY_REMOTE_NODES);

  master_sensors_init();
  master_gps_init();
  remote_nodes_init();

  connectWiFi();
  configTime(0, 0, NTP_SERVER); // UTC

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setKeepAlive(30);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
    connectWiFi();
  if (!mqtt.connected())
    connectMQTT();
  mqtt.loop();

  unsigned long now = millis();
  if (now - last_publish_ms >= PUBLISH_INTERVAL_MS)
  {
    last_publish_ms = now;

    String ts = isoTimestamp();

    NodeData nodes[NODE_COUNT];
    size_t n = buildNodeData(nodes, NODE_COUNT);

    Serial.printf("\n--- Publishing cycle (%u nodes) ---\n",
                  (unsigned)n);
    for (size_t i = 0; i < n; ++i)
    {
      publishNode(nodes[i], ts);
    }
  }
}