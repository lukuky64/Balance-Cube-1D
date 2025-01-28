#include <Arduino.h>
#include <SimpleFOC.h>
#include "State_Machine.hpp"

// configure the ADC for the pin
bool adcInit(uint8_t pin)
{
    static bool initialized = false;

    int8_t channel = digitalPinToAnalogChannel(pin);
    if (channel < 0)
    {
        ESP_LOGE("ERROR", "Not ADC pin");
        return false; // not adc pin
    }

    analogSetAttenuation(ADC_11db); // this is what causes : Pin is not configured as analog channel. They are also all 11dB by default
    analogReadResolution(12);
    pinMode(pin, ANALOG);
    analogSetPinAttenuation(pin, ADC_11db); // this is what causes : Pin is not configured as analog channel. They are also all 11dB by default
    analogRead(pin);
    return true;
}

void setup()
{
    Serial.begin(115200);

    adcInit(4);
    adcInit(5);
}

void loop()
{
    int value4 = analogRead(4); // GPIO4
    int value5 = analogRead(5); // GPIO5
    ESP_LOGI("TAG", "GPIO4: %d, GPIO5: %d", value4, value5);
    delay(500);
}