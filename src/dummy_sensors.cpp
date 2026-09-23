#include "sensor_interface.h"

#ifdef USE_DUMMY_SENSORS

#include <Arduino.h>
#include <math.h>

// Base values (approx. ambient room temperature)
static const float T1_BASE = 25.0f; // Sensor 1
static const float T2_BASE = 25.3f; // Sensor 2 (slightly offset -> will need calibration)

static unsigned long t_start = 0;

void sensors_init()
{
    t_start = millis();
    randomSeed(esp_random());
}

SensorReading sensors_read()
{
    float t = (millis() - t_start) / 1000.0f;

    // Slow sinusoidal "environmental drift" (±0.8 °C over 2-minute cycle)
    float drift = 0.8f * sinf(2.0f * PI * t / 120.0f);

    // Random noise around the base
    // Sensor 1 is noisier (mimics an unfiltered LM35 / thermistor)
    // Sensor 2 is quieter (mimics a DS18B20)
    float noise1 = (random(-100, 101) / 100.0f) * 0.20f; // ±0.20 °C
    float noise2 = (random(-100, 101) / 100.0f) * 0.05f; // ±0.05 °C

    SensorReading r;
    r.t1 = T1_BASE + drift + noise1;
    r.t2 = T2_BASE + drift + noise2;
    return r;
}

#endif // USE_DUMMY_SENSORS