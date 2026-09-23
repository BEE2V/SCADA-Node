#include "sensor_interface.h"

#ifndef USE_DUMMY_GPS

#include <Arduino.h>

// #include <TinyGPSPlus.h>
// static TinyGPSPlus gps;
// HardwareSerial GPSSerial(1);   // UART1 on XIAO ESP32S3

void master_gps_init()
{
    // GPSSerial.begin(9600, SERIAL_8N1, /*RX=*/7, /*TX=*/6);
}

GpsReading master_gps_read()
{
    GpsReading g = {0.0, 0.0, false};
    // while (GPSSerial.available()) gps.encode(GPSSerial.read());
    // if (gps.location.isValid()) {
    //     g.latitude  = gps.location.lat();
    //     g.longitude = gps.location.lng();
    //     g.valid     = true;
    // }
    return g;
}

#endif // !USE_DUMMY_GPS