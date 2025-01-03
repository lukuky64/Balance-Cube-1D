
#ifndef BLDC_HPP
#define BLDC_HPP

#include "Arduino.h"
#include "esp_log.h"

class BLDC
{
public:
    BLDC();
    ~BLDC();
    bool checkStatus();
    void enableMotor(bool enable);
    void begin();

private:
};

#endif // BLDC_HPP