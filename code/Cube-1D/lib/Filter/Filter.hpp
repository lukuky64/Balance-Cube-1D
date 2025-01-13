#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include "SemaphoreGuard.hpp"

class Filter
{
public:
    Filter(float processNoise, float measurementNoise, float estimatedError = 0.0f, float initialValue = 0.0f);

    void update(float measurement);
    void update(float measurement, float controlInput, float controlEffect); // With control input
    float getValue();

    // Setters
    void setProcessNoise(float processNoise);
    void setMeasurementNoise(float measurementNoise);
    void setEstimatedError(float estimatedError);

    // Getters
    float getProcessNoise();
    float getMeasurementNoise();
    float getEstimatedError();

    // Reset filter
    void reset(float initialValue, float initialError);

private:
    SemaphoreHandle_t m_dataMutex = nullptr;
    float Q; // Process noise covariance
    float R; // Measurement noise covariance
    float P; // Estimation error covariance
    float X; // State estimate
    float K; // Kalman Gain

    bool isInitialised; // Initialization flag
};
