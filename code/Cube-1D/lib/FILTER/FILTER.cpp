#include "FILTER.hpp"

FILTER::FILTER(float processNoise, float measurementNoise, float estimatedError, float initialValue)
    : Q(processNoise), R(measurementNoise), P(estimatedError), X(initialValue), K(0.0f), isInitialised(true) {}

void FILTER::update(float measurement)
{
    if (!isInitialised)
    {
        // Optionally handle uninitialized state
        return;
    }

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

void FILTER::update(float measurement, float controlInput, float controlEffect)
{
    if (!isInitialised)
    {
        // Optionally handle uninitialized state
        return;
    }

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

float FILTER::getValue() const
{
    return X;
}

// Setters
void FILTER::setProcessNoise(float processNoise) { Q = processNoise; }
void FILTER::setMeasurementNoise(float measurementNoise) { R = measurementNoise; }
void FILTER::setEstimatedError(float estimatedError) { P = estimatedError; }

// Getters
float FILTER::getProcessNoise() const { return Q; }
float FILTER::getMeasurementNoise() const { return R; }
float FILTER::getEstimatedError() const { return P; }

// Reset filter
void FILTER::reset(float initialValue, float initialError)
{
    X = initialValue;
    P = initialError;
    K = 0.0f;
    isInitialised = true;
}
