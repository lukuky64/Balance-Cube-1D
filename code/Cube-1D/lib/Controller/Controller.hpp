#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include "Devices.hpp"
#include "Filter.hpp"
#include "Estimator/Estimator.hpp"
#include "MinJerkController/MinJerkController.hpp"
#include "LQRController/LQRController.hpp"
#include "RateLimiter/RateLimiter.hpp"
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

    bool setup();
    bool checkStatus();
    bool getControllable();
    void updateData();
    void updateBalanceControl(float dt_ms);
    void updateBLDC();
    float getTargetAccel();
    float linearRegulator(float dt);

    float (&getDataBuffer())[log_columns];

    void setState();

    float LQRegulator(float dt);

    // void setTargetTau(float tau);
    // float getTargetTau();

private:
    void updateControlability();
    float SoftClamp(float u);
    bool setupFilters();

    SemaphoreHandle_t m_controllableMutex = nullptr;
    bool m_controlable;

    LQRController m_lqrController;
    MinJerkController m_minJerkController;

    RateLimiter m_rateLimiter;

    Devices &m_devicesRef;
    FILTERS m_filters;
    Estimator m_estimator;

    // SemaphoreHandle_t m_target_tau_mutex = nullptr;
    // float m_target_tau;

    float m_maxTau;

    // State for integral and derivative
    float integral_error;
    float previous_error;

    float m_dataBuffer[log_columns];
};