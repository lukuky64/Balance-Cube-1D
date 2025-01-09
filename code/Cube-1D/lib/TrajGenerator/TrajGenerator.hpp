#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include "Params.hpp"
#include "Devices.hpp"
#include "Filter.hpp"

#include "SemaphoreGuard.hpp"

struct stateDyamics
{
    float pos;
    float vel;
    float acc;
    float jer;
    float sna;
    float cra;
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
    void setTrajDyn(float pos, float vel, float acc, float jer, float sna, float cra);
    void setRefs(float theta_r, float omega_r, float alpha_r);

    SemaphoreHandle_t m_state_mutex = nullptr;
    stateDyamics m_stateDyn;

    SemaphoreHandle_t m_ref_mutex = nullptr;
    trajRefs m_Ref;

private:
};