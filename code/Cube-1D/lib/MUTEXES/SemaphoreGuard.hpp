#ifndef SEMAPHORE_GUARD_HPP
#define SEMAPHORE_GUARD_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

class SemaphoreGuard
{
public:
    SemaphoreGuard(SemaphoreHandle_t semaphore, TickType_t timeout = portMAX_DELAY)
        : m_semaphore(semaphore), m_acquired(false)
    {
        m_acquired = (xSemaphoreTake(m_semaphore, timeout) == pdTRUE);
    }

    ~SemaphoreGuard()
    {
        if (m_acquired)
        {
            xSemaphoreGive(m_semaphore);
        }
        else
        {
            ESP_LOGE("DEVICES", "Failed to take mutex");
        }
    }

    bool acquired() const { return m_acquired; }

private:
    SemaphoreHandle_t m_semaphore;
    bool m_acquired;
};

#endif // SEMAPHORE_GUARD_HPP