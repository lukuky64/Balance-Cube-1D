#include "Servo_CTR.hpp"

Servo_CTR::Servo_CTR()
{
}

Servo_CTR::~Servo_CTR()
{
    m_servo.detach(); // Turn the servo off
}

bool Servo_CTR::begin(uint8_t servoPin)
{
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    m_servoPin = servoPin;

    if (!m_servo.attached())
    {
        m_servo.setPeriodHertz(50);             // standard 50 hz servo
        m_servo.attach(m_servoPin, 1000, 2000); // pin, min, max
    }

    m_servo.write(0);

    return true;
}

void Servo_CTR::command(uint8_t angle)
{
    m_servo.write(angle);
}

bool Servo_CTR::checkStatus()
{
    return true;
}
