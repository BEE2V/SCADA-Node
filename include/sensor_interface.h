#pragma once

struct SensorReading
{
    float t1; // Sensor 1 (noisier, e.g. LM35/thermistor)
    float t2; // Sensor 2 (more accurate, e.g. DS18B20)
};

struct GpsReading
{
    double latitude;
    double longitude;
    bool valid;
};

// Implemented by dummy_* or real_* depending on build flags
void sensors_init();
SensorReading sensors_read();

void gps_init();
GpsReading gps_read();