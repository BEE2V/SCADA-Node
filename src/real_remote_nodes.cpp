#include "remote_nodes.h"

#ifndef USE_DUMMY_REMOTE_NODES

#include <Arduino.h>

// TODO: Fill in when the LoRa radio is ready.

void remote_nodes_init()
{
    // e.g. LoRa.begin(868E6);
}

size_t remote_nodes_read(NodeData *out, size_t max_count)
{
    (void)out;
    (void)max_count;
    return 0;
}

#endif // !USE_DUMMY_REMOTE_NODES