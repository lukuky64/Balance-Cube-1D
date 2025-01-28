#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include "Params.hpp"
#include "Devices.hpp"
#include "Filter.hpp"

#include "SemaphoreGuard.hpp"

struct Gains
{
    float k1{0.0f};
    float k2{0.0f};
    float k3{0.0f};
    float k4{0.0f};
};

// Attitude estimator class
class LQRController
{
public:
    // Constructor
    LQRController();

    ~LQRController();

    float generate(float theta, float theta_dot, float phi, float phi_dot);
    void setGains(float k1, float k2, float k3, float k4);

private:
    // LQR Gain Matrix (precomputed offline)
    Gains m_K;

    void WrapAngle(float &angle);
};