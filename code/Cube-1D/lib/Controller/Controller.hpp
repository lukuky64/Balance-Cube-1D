#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include "Devices.hpp"
#include "Filter.hpp"
#include "Estimator/Estimator.hpp"
#include "TrajGenerator/TrajGenerator.hpp"
#include "Params.hpp"
#include <algorithm>

// create a set of filters for each device
struct FILTERS
{
    Filter filter_theta;
    Filter filter_omega;
    Filter filter_motor_theta;
    Filter filter_motor_omega;
};

class Controller
{
public:
    Controller(Devices &devicesRef);

    ~Controller();

    void setup();
    bool checkStatus();
    bool getControllable();
    void updateData();
    void updateBalanceControl(float dt);
    void updateBLDC();
    float getTargetAccel();
    float linearRegulator(float dt);

    float (&getDataBuffer())[3];

    void setState();

    // void setTargetTau(float tau);
    // float getTargetTau();

private:
    void updateControlability();

    SemaphoreHandle_t m_controllableMutex = nullptr;
    bool m_controlable;

    Devices &m_devicesRef;
    FILTERS m_filters;
    Estimator m_estimator;
    TrajGenerator m_traj_gen;

    // SemaphoreHandle_t m_target_tau_mutex = nullptr;
    // float m_target_tau;

    // Moment of inertia
    const float m_wheel_J;
    float m_maxTau;

    // State for integral and derivative
    float integral_error;
    float previous_error;

    const float m_controllableAngleThreshold;

    float m_dataBuffer[log_columns];
};