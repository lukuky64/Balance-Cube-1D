#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include <SimpleFOC.h>
#include "Params.hpp"
#include "Comms/Comms.hpp"
#include "Mag_Enc/Mag_Enc.hpp"

class BLDC_CTR
{
public:
    BLDC_CTR();
    ~BLDC_CTR();
#if DUMMY_BLDC
    bool checkStatus() { return true; }
    void enableMotor(bool enable) { return; }
    bool begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, Mag_Enc mag_enc, float voltage) { return true; }
    void loopFOC() { return; }
    void moveTarget(float target) { return; }
    void updateVoltageLimits(float voltage) { return; }
    float getMaxTau() { return 2.0f; }
    float getTheta() { return 0.0f; }
    float getOmega() { return 10.0f; }
#else
    bool checkStatus();
    void enableMotor(bool enable);
    bool begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, Mag_Enc mag_enc, float voltage);
    void loopFOC();
    void moveTarget(float target);
    void updateVoltageLimits(float voltage);
    float getMaxTau();
    float getTheta();
    float getOmega();

#endif

private:
    BLDCMotor *m_motor;
    BLDCDriver3PWM *m_driver;
    Mag_Enc *m_sensor;
    InlineCurrentSense *m_current_sense;

    float m_voltage;

    void setMotorSettings();
};