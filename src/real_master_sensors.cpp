#include "sensor_interface.h"

#ifndef USE_DUMMY_SENSORS

#include <Arduino.h>

// TODO: Fill in with real hardware when ready.
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #define LM35_PIN     1     // XIAO ESP32S3 ADC pin (e.g. A0 / GPIO1)
// #define ONEWIRE_PIN  2     // any free GPIO
//
// static OneWire oneWire(ONEWIRE_PIN);
// static DallasTemperature ds(&oneWire);

void master_sensors_init()
{
    // ds.begin();
}

SensorReading master_sensors_read()
{
    SensorReading r = {NAN, NAN};
    // int raw = analogRead(LM35_PIN);
    // r.t1 = (raw / 4095.0f) * 3.3f * 100.0f;   // LM35: 10 mV/°C
    // ds.requestTemperatures();
    // r.t2 = ds.getTempCByIndex(0);
    return r;
}

#endif // !USE_DUMMY_SENSORS