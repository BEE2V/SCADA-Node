#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <string.h>

#include "config.h"
#include "node_data.h"
#include "remote_nodes.h"

static WiFiClient espClient;
static PubSubClient mqtt(espClient);

static unsigned long last_publish_ms = 0;
static unsigned long last_mqtt_attempt_ms = 0;
static unsigned long last_wifi_attempt_ms = 0;
static unsigned long last_heartbeat_ms = 0;

// ================= WiFi: NON-BLOCKING =================
static bool ensureWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
        return true;

    unsigned long now = millis();
    if (now - last_wifi_attempt_ms < WIFI_RETRY_MS)
        return false;
    last_wifi_attempt_ms = now;

    Serial.printf("[WiFi] Connecting to %s\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(50);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - start < WIFI_CONNECT_TIMEOUT_MS)
    {
        delay(200);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.printf("[WiFi] Connected. IP: %s\n",
                      WiFi.localIP().toString().c_str());
        return true;
    }

    Serial.println("[WiFi] Attempt failed. Will retry later.");
    return false;
}

// ================= MQTT: NON-BLOCKING =================
static void tryConnectMQTT()
{
    String clientId = String("xiao-gw-") +
                      String((uint32_t)esp_random(), HEX);
    Serial.print("[MQTT] Connecting... ");
    if (mqtt.connect(clientId.c_str()))
    {
        Serial.println("connected");
    }
    else
    {
        Serial.printf("failed rc=%d (retry in %lus)\n",
                      mqtt.state(), MQTT_RETRY_MS / 1000);
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

// ================= Publish =================
static void publishNode(const NodeData &n, const String &ts)
{
    JsonDocument doc;
    doc["node_id"] = n.node_id;
    doc["timestamp"] = ts;
    doc["latitude"] = n.latitude;
    doc["longitude"] = n.longitude;
    doc["gps_valid"] = n.valid;
    doc["t1"] = n.t1;
    doc["t2"] = n.t2;
    doc["temperature"] = n.temperature;
    doc["lora_rssi"] = n.lora_rssi;
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["source"] = "lora";
    doc["gateway"] = "xiao_master";

    char payload[MQTT_BUFFER_SIZE];
    size_t len = serializeJson(doc, payload, sizeof(payload));

    String topic = String(MQTT_TOPIC_PREFIX) + n.node_id + "/data";

    bool ok = mqtt.publish(topic.c_str(), payload, len);

    Serial.printf("[PUB] %-32s %s  |  T=%.2f  len=%u\n",
                  topic.c_str(),
                  ok ? "ok  " : "FAIL",
                  n.temperature,
                  (unsigned)len);
}

// ================= Setup =================
void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== EE2120 Master Gateway (XIAO ESP32S3) ===");
    Serial.printf("Remote nodes  : %u\n", (unsigned)REMOTE_NODE_COUNT);
    Serial.printf("Dummy LoRa    : %d\n", (int)USE_DUMMY_REMOTE_NODES);
    Serial.printf("Topic prefix  : %s\n", MQTT_TOPIC_PREFIX);
    Serial.printf("MQTT buffer   : %d bytes\n", MQTT_BUFFER_SIZE);

    remote_nodes_init();

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
    {
        delay(300);
        Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.printf("[WiFi] Connected. IP: %s\n",
                      WiFi.localIP().toString().c_str());
    }
    else
    {
        Serial.println("[WiFi] Not connected at boot — will retry in loop()");
    }

    configTime(0, 0, NTP_SERVER);

    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setKeepAlive(30);
    mqtt.setSocketTimeout(5);
    mqtt.setBufferSize(MQTT_BUFFER_SIZE); // ← critical fix

    randomSeed(esp_random());
}

// ================= Loop =================
void loop()
{
    unsigned long now = millis();

    // 1. WiFi keep-alive
    bool wifi_ok = ensureWiFi();

    // 2. MQTT keep-alive
    if (wifi_ok)
    {
        if (!mqtt.connected())
        {
            if (now - last_mqtt_attempt_ms >= MQTT_RETRY_MS)
            {
                last_mqtt_attempt_ms = now;
                tryConnectMQTT();
            }
        }
        else
        {
            mqtt.loop();
        }
    }

    // 3. Heartbeat
    if (now - last_heartbeat_ms >= HEARTBEAT_MS)
    {
        last_heartbeat_ms = now;
        Serial.printf("[HB] up=%lus  WiFi=%s  MQTT=%s  heap=%u\n",
                      now / 1000,
                      WiFi.status() == WL_CONNECTED ? "yes" : "no ",
                      mqtt.connected() ? "yes" : "no ",
                      (unsigned)ESP.getFreeHeap());
    }

    // 4. Publish cycle
    if (now - last_publish_ms >= PUBLISH_INTERVAL_MS)
    {
        last_publish_ms = now;

        if (WiFi.status() == WL_CONNECTED && mqtt.connected())
        {
            String ts = isoTimestamp();

            NodeData nodes[REMOTE_NODE_COUNT];
            size_t n = remote_nodes_read(nodes, REMOTE_NODE_COUNT);

            Serial.printf("\n--- Cycle: %u nodes ---\n", (unsigned)n);
            for (size_t i = 0; i < n; ++i)
            {
                publishNode(nodes[i], ts);
            }
        }
        else
        {
            Serial.println("[PUB] skipped (WiFi or MQTT down)");
        }
    }

    // 5. Watchdog yield
    delay(10);
}