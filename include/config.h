#pragma once

// ============================================================
//  3 remote nodes.
// ============================================================
struct RemoteNodeDef
{
    const char *node_id;
    double latitude;
    double longitude;
    float base_t1;
    float base_t2;
};

static const RemoteNodeDef REMOTE_NODE_DEFS[] = {
    // id          lat        lon        base_t1  base_t2
    {"node_00", 7.2555, 80.5960, 26.4f, 26.6f},
    {"node_01", 7.2530, 80.5990, 24.0f, 24.2f},
    {"node_02", 7.2548, 80.5995, 25.7f, 25.9f},
};

#define REMOTE_NODE_COUNT (sizeof(REMOTE_NODE_DEFS) / sizeof(REMOTE_NODE_DEFS[0]))

// ============================================================
//  WiFi (must be 2.4 GHz)
// ============================================================
#define WIFI_SSID "Dialog 4G"
#define WIFI_PASSWORD "Q06YLA0QED3"

// ============================================================
//  MQTT — LOCAL Mosquitto
//  Replace with your PC's IPv4 from `ipconfig`
// ============================================================
#define MQTT_BROKER "192.168.8.102"
#define MQTT_PORT 1883

// Short, unique prefix. Change `g07` to your group number.
#define MQTT_TOPIC_PREFIX "ee2120_g07/"

// ============================================================
//  Timing
// ============================================================
#define PUBLISH_INTERVAL_MS 5000UL
#define WIFI_RETRY_MS 10000UL
#define MQTT_RETRY_MS 3000UL
#define HEARTBEAT_MS 10000UL
#define WIFI_CONNECT_TIMEOUT_MS 8000UL
#define NTP_SERVER "pool.ntp.org"

// ============================================================
//  MQTT buffer size — MUST be larger than the longest JSON
//  payload + topic. 1024 gives comfortable headroom.
// ============================================================
#define MQTT_BUFFER_SIZE 1024