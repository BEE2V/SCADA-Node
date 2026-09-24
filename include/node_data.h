#pragma once
#include <stdint.h>

struct NodeData
{
    char node_id[16];
    double latitude;
    double longitude;
    float t1;
    float t2;
    float temperature;
    bool valid;
    int8_t lora_rssi;
};