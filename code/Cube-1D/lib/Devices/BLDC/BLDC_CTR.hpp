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
    bool begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, Mag_Enc *mag_enc, float voltage, float Kv) { return true; }
    void loopFOC() { return; }
    void moveTarget(float target) { return; }
    void updateVoltageLimits(float voltage) { return; }
    float getMaxTau() { return 0.3f; }
    float getTheta() { return 0.0f; }
    float getOmega() { return 10.0f; }
    float getTarget() { return 0.2f; }

#else
    bool checkStatus();
    void enableMotor(bool enable);
    bool begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, Mag_Enc *mag_enc, float voltage, float Kv);
    void loopFOC();
    void moveTarget(float target);
    void updateVoltageLimits(float voltage);
    float getMaxTau();
    float getTheta();
    float getOmega();
    float getTarget();

#endif

private:
    BLDCMotor *m_motor;
    BLDCDriver3PWM *m_driver;
    Mag_Enc *m_sensor;
    InlineCurrentSense *m_current_sense;

    void setTorqueConstant(float Kv);
    float torqueToCurrent(float tau);

    float m_Kv;          // Motor Kv rating, units should be rpm/V
    float m_max_current; // Maximum current limit

    float m_torque_constant; // Torque constant

    float m_voltage;

    void setMotorSettings();
};