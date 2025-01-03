
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

    void setup();
    void begin();
    void loop();

    void initialisationSeq();
    void calibrationSeq();
    void controlSeq();
    void lightSleepSeq();
    void idleSeq();
    void checkActivityTask();

    STATES getCurrentState();

    void criticalErrorSeq();

    // FreeRTOS Tasks
    static void indicationLoopTask(void *pvParameters);
    static void refreshStatusTask(void *pvParameters);

    static void updateFiltersLoopTask(void *pvParameters);
    static void balanceLoopTask(void *pvParameters);
    static void BLDCLoopTask(void *pvParameters);

private:
    SemaphoreHandle_t m_stateMutex = nullptr;
    STATES m_currState;
    DEVICES &m_devices = *new DEVICES();
    CONTROLLER m_control;

    // FreeRTOS Handles
    TaskHandle_t m_indicationLoopTaskHandle = nullptr;
    TaskHandle_t m_refreshStatusTaskHandle = nullptr;

    TaskHandle_t m_updateFiltersLoopTaskHandle = nullptr;
    TaskHandle_t m_balanceLoopTaskHandle = nullptr;
    TaskHandle_t m_BLDCLoopTaskHandle = nullptr;

    uint16_t m_refreshStatusPeriod;
    uint16_t m_indicationPeriod;
    uint16_t m_activityCheckPeriod;
};

#endif // STATE_MACHINE_HPP