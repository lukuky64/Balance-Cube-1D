#include "FILTER.hpp"

FILTER::FILTER(float processNoise, float measurementNoise, float estimatedError, float initialValue)
    : isInitialised(true)
{
    m_dataMutex = xSemaphoreCreateMutex();

    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        Q = processNoise;
        R = measurementNoise;
        P = estimatedError;
        X = initialValue;
        K = 0.0f;
    }
}

void FILTER::update(float measurement)
{
    if (!isInitialised)
    {
        // Optionally handle uninitialized state
        return;
    }

    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        // Prediction update
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

void FILTER::update(float measurement, float controlInput, float controlEffect)
{
    if (!isInitialised)
    {
        // Optionally handle uninitialized state
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

float FILTER::getValue()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return X;
    }
    return 0.0f; // or some other default value
}

// Setters
void FILTER::setProcessNoise(float processNoise) { Q = processNoise; }
void FILTER::setMeasurementNoise(float measurementNoise) { R = measurementNoise; }
void FILTER::setEstimatedError(float estimatedError) { P = estimatedError; }

// Getters
float FILTER::getProcessNoise()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return Q;
    }
    return 0.0f; // or some other default value
}

float FILTER::getMeasurementNoise()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return R;
    }
    return 0.0f; // or some other default value
}

float FILTER::getEstimatedError()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return P;
    }
    return 0.0f; // or some other default value
}

// Reset filter
void FILTER::reset(float initialValue, float initialError)
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
