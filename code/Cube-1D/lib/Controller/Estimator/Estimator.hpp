#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include "Params.hpp"
#include "Devices.hpp"
#include "Filter.hpp"

#include "SemaphoreGuard.hpp"

#define QUARTER_PI (PI / 4.0f);

// Attitude estimator class
class Estimator
{
public:
    // Constructor
    Estimator(Devices &devicesRef, uint16_t dt);

    // Estimate step
    void estimate();

    float getTheta();
    float getOmega();

    bool selectDevice();

    // Angular velocity bias calibration
    bool calibrate();

private:
    Devices &m_devicesRef;

    void estimateWithIMU();
    void estimateWithRot_Enc();

    bool calibrateStartSide();
    bool calibrateOmegaBias();

    // Predict step
    void predict(float omega_y);
    // Correct step
    void correct(float ax, float ay);
    // Angular velocity (rad/s) bias

    void WrapTheta();

    float m_startAngle;

    float m_theta; // Orientation angle (radians)
    float m_omega; // Angular velocity (rad/s)

    SemaphoreHandle_t m_theta_mutex = nullptr;
    SemaphoreHandle_t m_omega_mutex = nullptr;

    float m_omegaBias; // Angular velocity bias (rad/s)

    // Estimator gains for acceleration
    const float m_lds;

    bool m_imuSelected;
    bool m_rotEncSelected;

    uint16_t m_aquisition_dt;
};