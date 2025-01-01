

#ifndef SERVO_CTR_HPP
#define SERVO_CTR_HPP

#include "Arduino.h"
#include "esp_log.h"

class SERVO_CTR
{
public:
    SERVO_CTR();
    ~SERVO_CTR();
    bool checkStatus();

private:
};

#endif // SERVO_CTR_HPP