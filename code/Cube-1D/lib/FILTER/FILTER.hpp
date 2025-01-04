
#ifndef FILTER_HPP
#define FILTER_HPP

#include "Arduino.h"
#include "esp_log.h"
#include "SemaphoreGuard.hpp"

class FILTER
{
public:
    FILTER(float processNoise, float measurementNoise, float estimatedError, float initialValue);

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

#endif // KALMANFILTER_HPP
