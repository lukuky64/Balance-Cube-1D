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
    float rate = (input - m_prevInput) / dt; // Compute rate of change
    if (rate > m_limit)
        input = m_prevInput + m_limit * dt;
    else if (rate < -m_limit)
        input = m_prevInput - m_limit * dt;

    // Update the previous input
    m_prevInput = input;

    return input;
}