#include "sensor_interface.h"

#ifdef USE_DUMMY_REMOTE_NODES

#include <Arduino.h>
#include <math.h>
#include <string.h>
#include "config.h"

// ---------------------------------------------------------------
// Dummy generator for the 3 remote LoRa nodes.
//
// All nodes share the same slow environmental drift (so the
// spatial map shows a smooth gradient) but each has its own
// base temperature (microclimate) and small noise.
// ---------------------------------------------------------------

void remote_nodes_init() {}

size_t remote_nodes_read(NodeData *out, size_t max_count)
{
    size_t n = 0;
    float t = millis() / 1000.0f;

    // Shared environmental drift (±0.8 °C over a 2-min cycle)
    float drift = 0.8f * sinf(2.0f * PI * t / 120.0f);

    for (size_t i = 0; i < REMOTE_NODE_COUNT && n < max_count; ++i)
    {
        const NodeDefinition &def = NODE_DEFS[i + 1]; // skip master

        float noise1 = (random(-100, 101) / 100.0f) * 0.20f;
        float noise2 = (random(-100, 101) / 100.0f) * 0.05f;

        NodeData &d = out[n];
        strncpy(d.node_id, def.node_id, sizeof(d.node_id) - 1);
        d.node_id[sizeof(d.node_id) - 1] = '\0';

        d.latitude = def.latitude;
        d.longitude = def.longitude;
        d.sensors.t1 = def.base_t1 + drift + noise1;
        d.sensors.t2 = def.base_t2 + drift + noise2;
        d.fused = 0.0f; // filled in main.cpp via fuse_temperature()
        d.valid = true;
        d.is_master = false;
        ++n;
    }
    return n;
}

#endif // USE_DUMMY_REMOTE_NODES