#pragma once

#include <Arduino.h>
#include "esp_log.h"

/*

USE: TimerGuard guard("TimerGuard", "Operation");

*/

class TimerGuard
{
public:
    // Constructor takes optional log tag & operation name for easy identification
    TimerGuard(const char *logTag = "TimerGuard", const char *operationName = "Operation")
        : m_logTag(logTag), m_operationName(operationName)
    {
        m_startUS = micros();
    }

    // Destructor automatically logs the elapsed time
    ~TimerGuard()
    {
        unsigned long endUS = micros();
        float passedTimeUS = static_cast<float>(endUS - m_startUS);
        ESP_LOGI(m_logTag, "%s took: %.2f us", m_operationName, passedTimeUS);
    }

private:
    const char *m_logTag;
    const char *m_operationName;
    unsigned long m_startUS;
};
