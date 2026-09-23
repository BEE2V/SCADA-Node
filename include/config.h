#pragma once

// ============================================================
//  Node definitions (single source of truth)
//  Index 0   = MASTER (its own sensors + GPS)
//  Index 1..N = remote LoRa nodes
// ============================================================
struct NodeDefinition
{
    const char *node_id;
    double latitude;
    double longitude;
    float base_t1; // dummy base temp for sensor 1
    float base_t2; // dummy base temp for sensor 2
};

static const NodeDefinition NODE_DEFS[] = {
    // id          lat        lon        base_t1  base_t2
    {"node_00", 7.2541, 80.5976, 25.0f, 25.3f}, // MASTER
    {"node_01", 7.2555, 80.5960, 26.4f, 26.6f}, // remote 1
    {"node_02", 7.2530, 80.5990, 24.0f, 24.2f}, // remote 2
    {"node_03", 7.2548, 80.5995, 25.7f, 25.9f}, // remote 3
};

#define NODE_COUNT (sizeof(NODE_DEFS) / sizeof(NODE_DEFS[0]))
#define MASTER_NODE_INDEX 0
#define REMOTE_NODE_COUNT (NODE_COUNT - 1)

// ============================================================
//  WiFi credentials
// ============================================================
#define WIFI_SSID "Dialog 4G"
#define WIFI_PASSWORD "Q06YLA0QED3"

// ============================================================
//  MQTT broker  (local Mosquitto on your PC)
//  Replace with your PC's IPv4 address (ipconfig / ifconfig)
// ============================================================
#define MQTT_BROKER "192.168.8.102"
#define MQTT_PORT 1883
#define MQTT_TOPIC_PREFIX "ee2120/"

// ============================================================
//  Timing
// ============================================================
#define PUBLISH_INTERVAL_MS 5000UL
#define NTP_SERVER "pool.ntp.org"