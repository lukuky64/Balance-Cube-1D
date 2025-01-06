
#ifndef BLDC_CTR_HPP
#define BLDC_CTR_HPP

#include "Arduino.h"
#include "esp_log.h"
#include <SimpleFOC.h>
#include "Params.hpp"
#include "COMMS.hpp"

class BLDC_CTR
{
public:
    BLDC_CTR();
    ~BLDC_CTR();
    bool checkStatus();
    void enableMotor(bool enable);
    bool begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, SPICOM &SPI_BUS, float voltage);
    void loop(float target);
    void updateVoltageLimits(float voltage);

private:
    BLDCMotor *m_motor;
    BLDCDriver3PWM *m_driver;
    MagneticSensorSPI *m_sensor;
    InlineCurrentSense *m_current_sense;

    float m_voltage;

    SPICOM *m_SPI_BUS = nullptr;

    void setMotorSettings();
};

#endif // BLDC_CTR_HPP