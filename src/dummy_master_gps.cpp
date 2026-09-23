#include "sensor_interface.h"

#ifdef USE_DUMMY_GPS

#include <Arduino.h>
#include <math.h>
#include "config.h"

void master_gps_init() {}

GpsReading master_gps_read()
{
    // Master stays near its fixed coordinate with tiny drift,
    // so the map marker jitters realistically.
    float t = millis() / 1000.0f;

    GpsReading g;
    g.latitude = NODE_DEFS[MASTER_NODE_INDEX].latitude + 0.00005 * sinf(2.0f * PI * t / 300.0f);
    g.longitude = NODE_DEFS[MASTER_NODE_INDEX].longitude + 0.00005 * cosf(2.0f * PI * t / 300.0f);
    g.valid = true;
    return g;
}

#endif // USE_DUMMY_GPS