#pragma once

// ---------- Node identity ----------
// Change this per team member / group
#define NODE_ID "G2_node_01_"

// ---------- WiFi ----------
#define WIFI_SSID "Dialog 4G"
#define WIFI_PASSWORD "Q06YLA0QED3"

// ---------- MQTT ----------
#define MQTT_BROKER "192.168.8.102"
#define MQTT_PORT 1883
#define MQTT_TOPIC "ee2120/" NODE_ID "/data"

// ---------- Timing ----------
#define PUBLISH_INTERVAL_MS 5000UL // 5 seconds
#define NTP_SERVER "pool.ntp.org"