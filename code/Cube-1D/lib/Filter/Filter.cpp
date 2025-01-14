#include "FILTER.hpp"

Filter::Filter(float processNoise, float measurementNoise, float estimatedError, float initialValue)
    : isInitialised(true)
{
    m_dataMutex = xSemaphoreCreateMutex();

    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        Q = processNoise;     // Uuncertainty in the model or process
        R = measurementNoise; // Uncertainty in the measurement
        P = estimatedError;   // Uncertainty in the initial state
        X = initialValue;     // Initial state
        K = 0.0f;             // Kalman Gain
    }
}

void Filter::update(float measurement, float controlInput, float controlEffect)
{
    if (!isInitialised)
    {
        return;
    }

    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {

        // Prediction update with control input
        X = X + controlEffect * controlInput;
        P = P + Q;

        // Measurement update
        if ((P + R) == 0)
        {
            // Prevent division by zero
            K = 0;
        }
        else
        {
            K = P / (P + R);
        }
        X = X + K * (measurement - X);
        P = (1 - K) * P;
    }
}

float Filter::getValue()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return X;
    }
    return 0.0f; // or some other default value
}

// Setters
void Filter::setProcessNoise(float processNoise) { Q = processNoise; }
void Filter::setMeasurementNoise(float measurementNoise) { R = measurementNoise; }
void Filter::setEstimatedError(float estimatedError) { P = estimatedError; }

// Getters
float Filter::getProcessNoise()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return Q;
    }
    return 0.0f; // or some other default value
}

float Filter::getMeasurementNoise()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return R;
    }
    return 0.0f; // or some other default value
}

float Filter::getEstimatedError()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return P;
    }
    return 0.0f; // or some other default value
}

// Reset Filter
void Filter::reset(float initialValue, float initialError)
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        X = initialValue;
        P = initialError;
        K = 0.0f;
        isInitialised = true;
    }
}
