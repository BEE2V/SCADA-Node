#include "sensor_interface.h"

#ifdef USE_DUMMY_GPS

#include <Arduino.h>
#include <math.h>

// University of Peradeniya approximate coordinates
static const double GPS_LAT_BASE = 7.2541;
static const double GPS_LON_BASE = 80.5976;

static unsigned long t_start = 0;

void gps_init()
{
    t_start = millis();
}

GpsReading gps_read()
{
    float t = (millis() - t_start) / 1000.0f;

    // Slow circular drift of ~±0.0005° (~55 m radius)
    GpsReading g;
    g.latitude = GPS_LAT_BASE + 0.0005 * sinf(2.0f * PI * t / 300.0f);
    g.longitude = GPS_LON_BASE + 0.0005 * cosf(2.0f * PI * t / 300.0f);
    g.valid = true;
    return g;
}

#endif // USE_DUMMY_GPS