#include "remote_nodes.h"

#ifdef USE_DUMMY_REMOTE_NODES

#include <Arduino.h>
#include <math.h>
#include <string.h>
#include "config.h"

void remote_nodes_init()
{
    randomSeed(esp_random());
}

size_t remote_nodes_read(NodeData *out, size_t max_count)
{
    size_t n = 0;
    float t = millis() / 1000.0f;

    // Shared environmental drift (±0.8 °C over a 2-minute cycle)
    float drift = 0.8f * sinf(2.0f * PI * t / 120.0f);

    for (size_t i = 0; i < REMOTE_NODE_COUNT && n < max_count; ++i)
    {
        const RemoteNodeDef &def = REMOTE_NODE_DEFS[i];

        float noise1 = (random(-100, 101) / 100.0f) * 0.20f;
        float noise2 = (random(-100, 101) / 100.0f) * 0.05f;

        float t1 = def.base_t1 + drift + noise1;
        float t2 = def.base_t2 + drift + noise2;
        float fused = 0.35f * t1 + 0.65f * t2;

        NodeData &d = out[n];
        strncpy(d.node_id, def.node_id, sizeof(d.node_id) - 1);
        d.node_id[sizeof(d.node_id) - 1] = '\0';

        d.latitude = def.latitude;
        d.longitude = def.longitude;
        d.t1 = t1;
        d.t2 = t2;
        d.temperature = fused;
        d.valid = true;
        d.lora_rssi = (int8_t)(-60 - (int)(random(0, 30)));

        ++n;
    }
    return n;
}

#endif // USE_DUMMY_REMOTE_NODES