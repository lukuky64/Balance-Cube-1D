#pragma once

#include "Arduino.h"

#include "driver/gptimer.h"

#include "FreeRTOSConfig.h"

#include "freertos/FreeRTOS.h"
#include "freeRTOS/task.h"

#include "esp_sleep.h"

#include "esp_log.h"

#include "Devices.hpp"
#include "Controller.hpp"

#include "TimerGuard.hpp"
#include "Threading.hpp"
#include "SemaphoreGuard.hpp"

enum STATES
{
    INITIALISATION = 0,
    CRITICAL_ERROR = 1,
    CALIBRATION = 2,
    IDLE = 3,
    LIGHT_SLEEP = 4,
    CONTROL = 5,
};

class State_Machine
{
public:
    State_Machine();
    ~State_Machine();

    void begin();
    void loop();

    // void cpuUsageMonitorSeq();

    void printCpuUsage();

    void initialisationSeq();
    void calibrationSeq();
    void controlSeq();
    void lightSleepSeq();
    void idleSeq();
    // void checkActivityTask();
    STATES getCurrentState();
    void criticalErrorSeq();
    void logSeq();
    const char *stateToString(STATES state);

    // FreeRTOS Tasks
    static void taskManagerTask(void *pvParameters);
    static void indicationTask(void *pvParameters);
    static void refreshStatusTask(void *pvParameters);
    static void updateFiltersTask(void *pvParameters);
    static void balanceTask(void *pvParameters);
    static void BLDCTask(void *pvParameters);
    static void logTask(void *pvParameters);
    static void ControlabilityTask(void *pvParameters);
    static void checkControllableTask(void *pvParameters);

private:
    SemaphoreHandle_t m_stateMutex = NULL;

    // SemaphoreHandle_t m_controllableMutex = NULL;
    // bool m_controllable;

    STATES m_currState;
    Devices m_devices;
    Controller m_control;

    // FreeRTOS Handles
    TaskHandle_t m_taskManagerTaskHandle = NULL;
    TaskHandle_t m_indicationLoopTaskHandle = NULL;
    TaskHandle_t m_refreshStatusTaskHandle = NULL;

    TaskHandle_t m_updateFiltersTaskHandle = NULL;
    TaskHandle_t m_balanceTaskHandle = NULL;
    TaskHandle_t m_BLDCTaskHandle = NULL;

    TaskHandle_t m_logTaskHandle = NULL;

    TaskHandle_t m_cpuUsageTaskHandle = NULL;
};