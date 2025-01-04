
#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include "freertos/FreeRTOS.h"
#include "esp_sleep.h"
#include "SemaphoreGuard.hpp"
#include "threading.hpp"

#include "Arduino.h"
#include "esp_log.h"

#include "DEVICES.hpp"

#include "CONTROLLER.hpp"

enum STATES
{
    INITIALISATION = 0,
    CRITICAL_ERROR = 1,
    CALIBRATION = 2,
    IDLE = 3,
    LIGHT_SLEEP = 4,
    CONTROL = 5,
};

class STATE_MACHINE
{
public:
    STATE_MACHINE();
    ~STATE_MACHINE();

    void begin();
    void loop();

    void initialisationSeq();
    void calibrationSeq();
    void controlSeq();
    void lightSleepSeq();
    void idleSeq();
    void checkActivityTask();
    bool canSleep();

    STATES getCurrentState();

    void criticalErrorSeq();

    // FreeRTOS Tasks
    static void indicationTask(void *pvParameters);
    static void refreshStatusTask(void *pvParameters);
    static void updateFiltersTask(void *pvParameters);
    static void balanceTask(void *pvParameters);
    static void BLDCTask(void *pvParameters);
    static void logTask(void *pvParameters);

private:
    SemaphoreHandle_t m_stateMutex = NULL;
    STATES m_currState;
    DEVICES m_devices;
    CONTROLLER m_control;

    // FreeRTOS Handles
    TaskHandle_t m_indicationLoopTaskHandle = NULL;
    TaskHandle_t m_refreshStatusTaskHandle = NULL;

    TaskHandle_t m_updateFiltersTaskHandle = NULL;
    TaskHandle_t m_balanceTaskHandle = NULL;
    TaskHandle_t m_BLDCTaskHandle = NULL;

    TaskHandle_t m_logTaskHandle = NULL;
};

#endif // STATE_MACHINE_HPP