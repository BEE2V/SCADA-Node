#include "sensor_interface.h"

#ifndef USE_DUMMY_SENSORS

#include <Arduino.h>

// TODO: Fill in when your hardware is ready.
// Example layout for LM35 + DS18B20:
//
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #define LM35_PIN 34
// #define ONEWIRE_PIN 4
//
// static OneWire oneWire(ONEWIRE_PIN);
// static DallasTemperature ds(&oneWire);

void sensors_init()
{
    // TODO: initialise ADC pin, OneWire bus, etc.
}

SensorReading sensors_read()
{
    SensorReading r = {NAN, NAN};
    // TODO:
    // int raw = analogRead(LM35_PIN);
    // r.t1 = (raw / 4095.0f) * 3.3f * 100.0f;   // LM35: 10 mV/°C
    // ds.requestTemperatures();
    // r.t2 = ds.getTempCByIndex(0);
    return r;
}

#endif // !USE_DUMMY_SENSORS