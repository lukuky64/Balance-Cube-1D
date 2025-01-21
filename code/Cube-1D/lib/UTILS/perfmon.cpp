#include "perfmon.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "sdkconfig.h"
#include "esp_log.h"

// Logger tag
static const char *TAG = "perfmon";

// Idle call counters for CPU 0 and CPU 1
static uint64_t idle0Calls = 0;
static uint64_t idle1Calls = 0;

#define CONFIG_ESP32_DEFAULT_CPU_FREQ_240

// Max idle calls based on CPU frequency
#if defined(CONFIG_ESP32_DEFAULT_CPU_FREQ_240)
constexpr uint64_t MaxIdleCalls = 1855000;
#elif defined(CONFIG_ESP32_DEFAULT_CPU_FREQ_160)
constexpr uint64_t MaxIdleCalls = 1233100;
#else
constexpr uint64_t MaxIdleCalls = 1500000; // Default value
#endif

// Idle task for CPU 0
static bool idle_task_0()
{
    ++idle0Calls;
    return false;
}

// Idle task for CPU 1
static bool idle_task_1()
{
    ++idle1Calls;
    return false;
}

// Performance monitoring task
static void perfmon_task(void *args)
{
    while (true)
    {
        float idle0 = static_cast<float>(idle0Calls);
        float idle1 = static_cast<float>(idle1Calls);

        idle0Calls = 0;
        idle1Calls = 0;

        int cpu0 = static_cast<int>(100.f - idle0 / MaxIdleCalls * 100.f);
        int cpu1 = static_cast<int>(100.f - idle1 / MaxIdleCalls * 100.f);

        ESP_LOGE(TAG, "Core 0 at %d%%", cpu0);
        ESP_LOGE(TAG, "Core 1 at %d%%", cpu1);

        // TODO: Make delay configurable
        vTaskDelay(pdMS_TO_TICKS(2000));
        // log_task_info();
    }
    vTaskDelete(nullptr);
}

// void log_task_info()
// {
//     UBaseType_t uxArraySize = uxTaskGetNumberOfTasks();
//     TaskStatus_t *pxTaskStatusArray;
//     uint32_t ulTotalRunTime;

//     pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
//     if (pxTaskStatusArray == NULL)
//     {
//         ESP_LOGE("TaskInfo", "Failed to allocate memory for task status array");
//         return;
//     }

//     uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
//     if (ulTotalRunTime == 0)
//     {
//         ulTotalRunTime = 1; // Prevent division by zero
//     }

//     for (UBaseType_t i = 0; i < uxArraySize; i++)
//     {
//         uint32_t taskPercentage = (pxTaskStatusArray[i].ulRunTimeCounter * 100UL) / ulTotalRunTime;
//         ESP_LOGE("TaskInfo", "Task: %s, Core: %d, Runtime: %u, CPU Usage: %u%%",
//                  pxTaskStatusArray[i].pcTaskName,
//                  pxTaskStatusArray[i].xCoreID,
//                  pxTaskStatusArray[i].ulRunTimeCounter,
//                  taskPercentage);
//     }

//     vPortFree(pxTaskStatusArray);
// }

// Start performance monitoring
esp_err_t perfmon_start()
{
    if (esp_register_freertos_idle_hook_for_cpu(idle_task_0, 0) == ESP_OK)
    {
        ESP_LOGE(TAG, "Idle task 0 hook registered successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to register idle task 0 hook");
    }

    if (esp_register_freertos_idle_hook_for_cpu(idle_task_1, 1) == ESP_OK)
    {
        ESP_LOGE(TAG, "Idle task 1 hook registered successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to register idle task 1 hook");
    }

    if (xTaskCreate(perfmon_task, "perfmon", 4096, NULL, 1, NULL) == pdPASS)
    {
        ESP_LOGE(TAG, "Perfmon task created successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to create perfmon task");
    }

    return ESP_OK;
}
