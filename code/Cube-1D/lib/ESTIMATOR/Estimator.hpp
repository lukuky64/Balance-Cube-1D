#ifndef estimator_attitude_h
#define estimator_attitude_h

#include "Arduino.h"
#include "esp_log.h"
#include "Params.hpp"
#include "Devices.hpp"
#include "Filter.hpp"

#include "SemaphoreGuard.hpp"

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

private:
    Devices &m_devicesRef;

    void estimateIMU();
    void estimateROT_ENC();

    // Angular velocity bias calibration
    void calibrate();
    // Predict step
    void predict(float omega_y);
    // Correct step
    void correct(float ax, float ay);
    // Angular velocity (rad/s) bias

    void WrapTheta();

    float m_theta; // Orientation angle (radians)
    float m_omega; // Angular velocity (rad/s)

    SemaphoreHandle_t m_theta_mutex = nullptr;
    SemaphoreHandle_t m_omega_mutex = nullptr;

    float m_omegaBias; // Angular velocity bias (rad/s)

    // Estimator gains
    float m_lds;
    float m_ldw;

    bool m_imuSelected;
    bool m_rotEncSelected;

    uint16_t m_dt;
};

#endif