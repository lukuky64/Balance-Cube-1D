#pragma once

#include "Arduino.h"
#include "esp_log.h"

class RateLimiter
{
public:
    RateLimiter();
    ~RateLimiter();

    float limit(float input, float dt);

    void reset();
    void setLimit(float limit);

private:
    float m_limit;
    float m_prevInput;
};