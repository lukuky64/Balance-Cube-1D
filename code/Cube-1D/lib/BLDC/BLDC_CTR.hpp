
#ifndef BLDC_CTR_HPP
#define BLDC_CTR_HPP

#include "Arduino.h"
#include "esp_log.h"
#include <SimpleFOC.h>
#include "Params.hpp"
#include "COMMS.hpp"
#include "Mag_Enc.hpp"

class BLDC_CTR
{
public:
    BLDC_CTR();
    ~BLDC_CTR();
#if DUMMY_BLDC
    bool checkStatus() { return true; }
    void enableMotor(bool enable) { return; }
    bool begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, Mag_Enc mag_enc, float voltage) { return true; }
    void loop(float target) { return; }
    void updateVoltageLimits(float voltage) { return; }
#else
    bool checkStatus();
    void enableMotor(bool enable);
    bool begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, Mag_Enc mag_enc, float voltage);
    void loop(float target);
    void updateVoltageLimits(float voltage);
#endif

private:
    BLDCMotor *m_motor;
    BLDCDriver3PWM *m_driver;
    Mag_Enc *m_sensor;
    InlineCurrentSense *m_current_sense;

    float m_voltage;

    void setMotorSettings();
};

#endif // BLDC_CTR_HPP