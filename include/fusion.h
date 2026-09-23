#pragma once
#include "sensor_interface.h"

// Weighted average + EMA smoothing of t1 and t2.
float fuse_temperature(const SensorReading &s);
void fusion_set_slot(int slot);