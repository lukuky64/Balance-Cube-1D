#include "RateLimiter.hpp"

RateLimiter::RateLimiter() : m_limit(0.0f)
{
    reset();
}

RateLimiter::~RateLimiter()
{
    //
}

void RateLimiter::reset()
{
    m_prevInput = 0.0f;
}

void RateLimiter::setLimit(float limit)
{
    m_limit = limit;
}

float RateLimiter::limit(float input, float dt)
{
    // Apply rate limiting
    float max_rate = m_limit; // Maximum rate of change (Nm/s)

    float rate = (input - m_prevInput) / dt; // Compute rate of change
    if (rate > max_rate)
        input = m_prevInput + max_rate * dt;
    else if (rate < -max_rate)
        input = m_prevInput - max_rate * dt;

    // Update the previous input
    m_prevInput = input;

    return input;
}