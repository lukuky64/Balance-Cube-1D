#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include <ESP32Servo.h>

class Servo_CTR
{
public:
    Servo_CTR();
    ~Servo_CTR();
    bool begin(uint8_t servoPin);
    bool checkStatus();
    void command(uint8_t angle);

private:
    uint8_t m_servoPin;
    Servo m_servo;
};