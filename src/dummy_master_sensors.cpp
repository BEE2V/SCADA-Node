#include "sensor_interface.h"

#ifdef USE_DUMMY_SENSORS

#include <Arduino.h>
#include <math.h>
#include "config.h"

// Master's own dummy sensors, using NODE_DEFS[0] base values
static const float T1_BASE = NODE_DEFS[0].base_t1;
static const float T2_BASE = NODE_DEFS[0].base_t2;

void master_sensors_init()
{
    // nothing to do for dummy
}

SensorReading master_sensors_read()
{
    float t = millis() / 1000.0f;

    // Slow sinusoidal environmental drift (±0.8 °C over a 2-min cycle)
    float drift = 0.8f * sinf(2.0f * PI * t / 120.0f);

    // Sensor 1 noisier, sensor 2 quieter
    float noise1 = (random(-100, 101) / 100.0f) * 0.20f; // ±0.20 °C
    float noise2 = (random(-100, 101) / 100.0f) * 0.05f; // ±0.05 °C

    SensorReading r;
    r.t1 = T1_BASE + drift + noise1;
    r.t2 = T2_BASE + drift + noise2;
    return r;
}

#endif // USE_DUMMY_SENSORS