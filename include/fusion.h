#pragma once
#include "sensor_interface.h"

// Returns fused temperature estimate using weighted averaging + EMA smoothing.
float fuse_temperature(const SensorReading &s);