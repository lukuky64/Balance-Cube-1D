

#ifndef SERVO_CTR_HPP
#define SERVO_CTR_HPP

#include "Arduino.h"
#include "esp_log.h"
#include <ESP32Servo.h>

class SERVO_CTR
{
public:
    SERVO_CTR();
    ~SERVO_CTR();
    bool begin(uint8_t servoPin);
    bool checkStatus();
    void command(uint8_t angle);

private:
    uint8_t m_servoPin;
    Servo m_servo;
};

#endif // SERVO_CTR_HPP