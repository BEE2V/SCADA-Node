#pragma once
#include <stdint.h>
#include <stddef.h>

struct SensorReading
{
    float t1;
    float t2;
};

struct GpsReading
{
    double latitude;
    double longitude;
    bool valid;
};

struct NodeData
{
    char node_id[16];
    double latitude;
    double longitude;
    SensorReading sensors;
    float fused;
    bool valid;
    bool is_master; // true for master, false for LoRa nodes
};

// ----- Master's own sensors (dummy or real) -----
void master_sensors_init();
SensorReading master_sensors_read();

// ----- Master's own GPS (dummy or real) -----
void master_gps_init();
GpsReading master_gps_read();

// ----- Remote nodes via LoRa (dummy or real) -----
void remote_nodes_init();
size_t remote_nodes_read(NodeData *out, size_t max_count);