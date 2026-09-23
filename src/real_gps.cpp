#include "sensor_interface.h"

#ifndef USE_DUMMY_GPS

#include <Arduino.h>
// #include <TinyGPSPlus.h>
// static TinyGPSPlus gps;
// HardwareSerial GPS(2);

void gps_init()
{
    // GPS.begin(9600, SERIAL_8N1, 16, 17);
}

GpsReading gps_read()
{
    GpsReading g = {0.0, 0.0, false};
    // while (GPS.available()) gps.encode(GPS.read());
    // if (gps.location.isValid()) {
    //     g.latitude  = gps.location.lat();
    //     g.longitude = gps.location.lng();
    //     g.valid     = true;
    // }
    return g;
}

#endif // !USE_DUMMY_GPS