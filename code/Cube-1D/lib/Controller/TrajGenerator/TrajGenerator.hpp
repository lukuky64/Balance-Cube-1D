#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include "Params.hpp"
#include "Devices.hpp"
#include "Filter.hpp"

#include "SemaphoreGuard.hpp"

// 5th-order polynomial
struct PolyCoeffs
{
    float a0{0.0f};
    float a1{0.0f};
    float a2{0.0f};
    float a3{0.0f};
    float a4{0.0f};
    float a5{0.0f};
};

struct trajRefs
{
    float theta_r;
    float omega_r;
    float alpha_r;
};

// Attitude estimator class
class TrajGenerator
{
public:
    // Constructor
    TrajGenerator();

    ~TrajGenerator();

    // Estimate steps
    void generate(float dt);

    float getTheta();
    float getOmega();

    bool selectDevice();

    trajRefs getRefs();
    // void setTrajDyn(float pos, float vel, float acc, float jer, float sna, float cra);
    void setRefs(float theta_r, float omega_r, float alpha_r);

    void setTargetAngle(float startAngle, float targetAngle, float totalTime);

    float m_elapsed{0.0f};
    float m_totalTime{1.0f}; // Some defaults

    SemaphoreHandle_t m_coeff_mutex = nullptr;
    PolyCoeffs m_coeffs;

    SemaphoreHandle_t m_ref_mutex = nullptr;
    trajRefs m_Ref;

private:
};