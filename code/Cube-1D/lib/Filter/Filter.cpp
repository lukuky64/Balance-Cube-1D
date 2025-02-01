#include "Filter.hpp"

Filter::Filter(float processNoise, float measurementNoise, float estimatedError, float initialValue)
    : isInitialised(true)
{
    m_dataMutex = xSemaphoreCreateMutex();

    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        m_vars.Q = processNoise;     // Uuncertainty in the model or process
        m_vars.R = measurementNoise; // Uncertainty in the measurement
        m_vars.P = estimatedError;   // Uncertainty in the initial state
        m_vars.X = initialValue;     // Initial state
        m_vars.K = 0.0f;             // Kalman Gain
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
        m_vars.X = m_vars.X + controlEffect * controlInput;
        m_vars.P = m_vars.P + m_vars.Q;

        // Measurement update
        if ((m_vars.P + m_vars.R) == 0)
        {
            // Prevent division by zero
            m_vars.K = 0;
        }
        else
        {
            m_vars.K = m_vars.P / (m_vars.P + m_vars.R);
        }
        m_vars.X = m_vars.X + m_vars.K * (measurement - m_vars.X);
        m_vars.P = (1 - m_vars.K) * m_vars.P;
    }
}

void Filter::computeMeasurementVariance(float measurement, bool lastData)
{
    // Accumulate sums
    m_sum += measurement;
    m_sumSq += measurement * measurement;
    m_count++;

    // If this is the last data point of a batch
    if (lastData)
    {
        SemaphoreGuard guard(m_dataMutex);
        if (guard.acquired() && m_count > 1)
        {
            // Population variance calcs
            float mean = m_sum / m_count;
            float meanSq = m_sumSq / m_count;
            float varPop = meanSq - (mean * mean);

            // Sample variance (unbiased) for smaller sample sizes
            float varSample = varPop * (static_cast<float>(m_count) / (m_count - 1));

            m_vars.R = varSample; // may as well use sample variance formula for better accuracy

            // Reset accumulations
            m_sum = 0.0f;
            m_sumSq = 0.0f;
            m_count = 0;
        }
        else if (guard.acquired())
        {
            // Edge case
            m_sum = 0.0f;
            m_sumSq = 0.0f;
            m_count = 0;
        }
    }
}

float Filter::getValue()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return m_vars.X;
    }
    return 0.0f; // or some other default value
}

// Setters
void Filter::setProcessNoise(float processNoise) { m_vars.Q = processNoise; }
void Filter::setMeasurementNoise(float measurementNoise) { m_vars.R = measurementNoise; }
void Filter::setEstimatedError(float estimatedError) { m_vars.P = estimatedError; }

// Getters
float Filter::getProcessNoise()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return m_vars.Q;
    }
    return 0.0f; // or some other default value
}

float Filter::getMeasurementNoise()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return m_vars.R;
    }
    return 0.0f; // or some other default value
}

void Filter::setR(float R)
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        m_vars.R = R;
    }
}

float Filter::getEstimatedError()
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        return m_vars.P;
    }
    return 0.0f; // or some other default value
}

// Reset Filter
void Filter::reset(float initialValue, float initialError)
{
    SemaphoreGuard guard(m_dataMutex);
    if (guard.acquired())
    {
        m_vars.X = initialValue;
        m_vars.P = initialError;
        m_vars.K = 0.0f;
        isInitialised = true;
    }
}
