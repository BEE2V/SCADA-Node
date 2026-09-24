#pragma once
#include <stddef.h>
#include "node_data.h"

void remote_nodes_init();
size_t remote_nodes_read(NodeData *out, size_t max_count);