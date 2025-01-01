
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

private:
};

#endif // BLDC_HPP