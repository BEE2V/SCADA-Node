#include "sensor_interface.h"

#ifndef USE_DUMMY_REMOTE_NODES

#include <Arduino.h>

// TODO: When your LoRa link is ready, fill this in.
//  - Poll each of the 3 remote nodes
//  - Parse their packets into NodeData entries
//  - Return the count of nodes read this cycle

void remote_nodes_init()
{
    // LoRa.begin(...);
}

size_t remote_nodes_read(NodeData *out, size_t max_count)
{
    (void)out;
    (void)max_count;
    return 0; // stub
}

#endif // !USE_DUMMY_REMOTE_NODES