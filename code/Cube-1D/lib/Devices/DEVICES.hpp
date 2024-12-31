
#ifndef DEVICES_HPP
#define DEVICES_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "Arduino.h"

#include "INDICATORS.hpp"

class DEVICES
{
public:
    DEVICES();
    bool setupIndication(bool silentIndication);
    bool setupVoltage();
    bool setupBLDC();
    bool setupIMU();
    bool setupROT_ENC();
    bool setupMAG();
    bool setupSerialLog();
    bool setupSDLog();
    bool setupServo();

private:
};

#endif // DEVICES_HPP