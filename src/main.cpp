#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

#include "config.h"
#include "sensor_interface.h"
#include "fusion.h"

static WiFiClient espClient;
static PubSubClient mqtt(espClient);

static unsigned long last_publish_ms = 0;

// ---------------- WiFi ----------------
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

// ---------------- MQTT ----------------
static void connectMQTT()
{
  while (!mqtt.connected())
  {
    Serial.print("[MQTT] Connecting... ");
    String clientId = String("esp32-") + NODE_ID + "-" +
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

// ---------------- Time ----------------
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

// ---------------- Publish ----------------
static void publishReading()
{
  SensorReading s = sensors_read();
  GpsReading g = gps_read();
  float fused = fuse_temperature(s);

  JsonDocument doc;
  doc["node_id"] = NODE_ID;
  doc["timestamp"] = isoTimestamp();
  doc["latitude"] = serialized(String(g.latitude, 6));
  doc["longitude"] = serialized(String(g.longitude, 6));
  doc["gps_valid"] = g.valid;
  doc["t1"] = serialized(String(s.t1, 3));
  doc["t2"] = serialized(String(s.t2, 3));
  doc["temperature"] = serialized(String(fused, 3));
  doc["rssi"] = WiFi.RSSI();

  char payload[512];
  size_t n = serializeJson(doc, payload, sizeof(payload));

  Serial.printf("[PUB] %s\n", payload);

  if (!mqtt.publish(MQTT_TOPIC, payload, n))
  {
    Serial.println("[MQTT] publish failed");
  }
}

// ---------------- Arduino entry points ----------------
void setup()
{
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== EE2120 Distributed Temperature Node ===");
  Serial.printf("Node ID     : %s\n", NODE_ID);
  Serial.printf("Dummy mode  : sensors=%d gps=%d\n",
                (int)USE_DUMMY_SENSORS, (int)USE_DUMMY_GPS);

  sensors_init();
  gps_init();

  connectWiFi();
  configTime(0, 0, NTP_SERVER); // UTC

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setKeepAlive(30);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connectWiFi();
  }
  if (!mqtt.connected())
  {
    connectMQTT();
  }
  mqtt.loop();

  unsigned long now = millis();
  if (now - last_publish_ms >= PUBLISH_INTERVAL_MS)
  {
    last_publish_ms = now;
    publishReading();
  }
}