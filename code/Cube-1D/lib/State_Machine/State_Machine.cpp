#include "State_Machine.hpp"

State_Machine::State_Machine() : m_control(m_devices)
{
    m_stateMutex = xSemaphoreCreateMutex();
}

State_Machine::~State_Machine()
{
}

void State_Machine::begin()
{
    {
        SemaphoreGuard guard(m_stateMutex);
        if (guard.acquired())
        {
            m_currState = INITIALISATION;
        }
    }

    while (true)
    {
        loop();
        vTaskDelay(pdMS_TO_TICKS(40)); // so we don't hog the CPU
    }
}

void State_Machine::loop()
{
    STATES currState;

    {
        SemaphoreGuard guard(m_stateMutex);
        if (guard.acquired())
        {
            currState = m_currState;
        }
    }

    switch (currState)
    {
    case INITIALISATION:
    {
        initialisationSeq();
    }
    break;
    case CRITICAL_ERROR:
    {
        criticalErrorSeq();
    }
    break;
    case CALIBRATION:
    {
        calibrationSeq();
        logSeq(); // initial logging start
    }
    break;
    case IDLE:
    {
        idleSeq();
    }
    break;
    case LIGHT_SLEEP:
    {
        lightSleepSeq();
        logSeq(); // go back to logging after sleep
    }
    break;
    case CONTROL:
    {
        controlSeq();
    }
    break;
    default:
        break;
    }
}

void State_Machine::BLDCTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    machine->m_devices.m_bldc.enableMotor(true); // Enable motor

    unsigned long startUS;
    unsigned long endUS = micros();

    while (machine->m_control.getControllable())
    {
        {
            // TimerGuard guard("State_Machine", "update BLDC task");
            machine->m_control.updateBLDC(); // currently only takes ~ 5uS or 200kHz
        }

        vTaskDelay(pdMS_TO_TICKS(BLDC_dt_ms));
    }

    machine->m_devices.m_bldc.enableMotor(false); // Disable motor

    // go to idle state
    {
        SemaphoreGuard guard(machine->m_stateMutex);
        if (guard.acquired())
        {
            machine->m_currState = IDLE;
        }
    }

    ESP_LOGI("State_Machine", "Ending BLDC Task!");

    // Delete the task explicitly
    machine->m_BLDCTaskHandle = NULL;
    vTaskDelete(NULL);
}

void State_Machine::balanceTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    machine->m_control.setState();

    while (true)
    {
        machine->m_control.updateBalanceControl(balance_dt_ms); // currently only takes ~ 27uS or 33kHz
        vTaskDelay(pdMS_TO_TICKS(balance_dt_ms));
    }
}

void State_Machine::updateFiltersTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    while (true)
    {
        machine->m_control.updateData();
        vTaskDelay(pdMS_TO_TICKS(aquisition_dt_ms));
    }
}

void State_Machine::indicationTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    while (true)
    {
        bool requirementsMet = machine->m_devices.indicateStatus();

        if (!requirementsMet)
        {
            {
                SemaphoreGuard guard(machine->m_stateMutex);
                if (guard.acquired())
                {
                    machine->m_currState = CRITICAL_ERROR;
                }
            }
        }

        // UBaseType_t uxHighWaterMark;
        // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        // printf("Remaining stack: %u words\n", uxHighWaterMark);

        vTaskDelay(pdMS_TO_TICKS(indication_dt_ms));
    }
}

void State_Machine::refreshStatusTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(refreshStatus_dt_ms));
        machine->m_devices.refreshStatusAll();
    }
}

void State_Machine::logTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    ESP_LOGI("State_Machine", "Starting log Task apparently");

    machine->m_devices.m_logger.startNewLog();

    // unsigned long startUS;
    // unsigned long endUS = micros();

    while (true)
    {
        // startUS = micros();
        machine->m_devices.m_logger.logData(machine->m_control.getDataBuffer(), log_columns); // around 160 microseconds without flush (3 float points and time), ~37ms for flush (4kB)
        // endUS = micros();
        // float passed_time_us = (endUS - startUS);
        // ESP_LOGI("State_Machine", "Logging Time: %f us", passed_time_us);
        vTaskDelay(pdMS_TO_TICKS(log_dt_ms));
    }
}

// void State_Machine::ControlabilityTask(void *pvParameters)
// {
//     vTaskDelay(pdMS_TO_TICKS(100));
//     // Convert generic pointer back to State_Machine*
//     auto *machine = static_cast<State_Machine *>(pvParameters);

//     while (true)
//     {

//         if (machine->m_control.controllableAngle())
//         {

//         }
//         vTaskDelay(pdMS_TO_TICKS(checkBoundary_dt_ms));
//     }

// }

void State_Machine::initialisationSeq()
{
    STATES currState = m_devices.init(LOG_SD, LOG_SERIAL, SILENT_INDICATION, SERVO_BRAKING, USE_IMU, USE_ROT_ENC) ? CALIBRATION : CRITICAL_ERROR;

    {
        SemaphoreGuard guard(m_stateMutex);
        if (guard.acquired())
        {
            m_currState = currState;
        }
    }

    if (m_indicationLoopTaskHandle == NULL)
    {
        xTaskCreate(&State_Machine::indicationTask, "Indication Loop Task", 2048, this, PRIORITY_LOW, &m_indicationLoopTaskHandle); // uses 1836 bytes of stack
    }
    if (m_refreshStatusTaskHandle == NULL)
    {
        xTaskCreate(&State_Machine::refreshStatusTask, "Device status check loop Task", 2048, this, PRIORITY_LOW, &m_refreshStatusTaskHandle);
    }
}

void State_Machine::criticalErrorSeq()
{
    ESP_LOGE("State_Machine CRITICAL ERROR", "Critical Error Sequence!");

    // destroy task
    vTaskDelete(m_refreshStatusTaskHandle);
    m_refreshStatusTaskHandle = NULL; // clear the handle

    vTaskDelay(pdMS_TO_TICKS(5000)); // wait for 5 seconds before restarting the initialisation sequence

    // destroy task
    vTaskDelete(m_indicationLoopTaskHandle);
    m_indicationLoopTaskHandle = NULL; // clear the handle

    {
        SemaphoreGuard guard(m_stateMutex);
        if (guard.acquired())
        {
            m_currState = INITIALISATION;
        }
    }
}

void State_Machine::calibrationSeq()
{
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI("State_Machine CALIBRATION", "Calibration Sequence!");

    bool calibrated = m_devices.calibrateSeq();

    vTaskDelay(pdMS_TO_TICKS(100));

    if (m_updateFiltersTaskHandle == NULL)
    {
        m_control.setup();
        xTaskCreate(&State_Machine::updateFiltersTask, "Starting Filters Task", 4096, this, PRIORITY_HIGH, &m_updateFiltersTaskHandle);
    }

    SemaphoreGuard guard(m_stateMutex);
    if (guard.acquired())
    {
        m_currState = (calibrated) ? IDLE : CRITICAL_ERROR;
    }
}

void State_Machine::lightSleepSeq()
{
    if (m_logTaskHandle != NULL)
    { // stop the log task before sleeping

        // flush the buffer first
        m_devices.m_logger.forceFlush();

        vTaskDelete(m_logTaskHandle);
        m_logTaskHandle = NULL;
    }

    if (m_devices.sleepMode())
    {
        ESP_LOGI("State_Machine", "Entering Light Sleep!");
        esp_light_sleep_start(); // Enter Light Sleep

        ESP_LOGI("State_Machine", "Waking up from light sleep!");
        m_devices.wakeMode();
    }

    SemaphoreGuard guard(m_stateMutex);
    if (guard.acquired())
    {
        m_currState = CONTROL;
    }
}

void State_Machine::controlSeq()
{
    ESP_LOGI("State_Machine", "Control Sequence!");

    if (m_balanceTaskHandle == NULL)
    {
        ESP_LOGI("State_Machine", "Starting balance Task");
        xTaskCreate(&State_Machine::balanceTask, "Starting balance Task", 4096, this, PRIORITY_HIGH, &m_balanceTaskHandle);
    }

    if (m_BLDCTaskHandle == NULL)
    {
        ESP_LOGI("State_Machine", "Starting BLDC Task");
        xTaskCreate(&State_Machine::BLDCTask, "Starting BLDC Task", 4096, this, PRIORITY_HIGH, &m_BLDCTaskHandle);
    }
}

void State_Machine::logSeq()
{
    if (m_logTaskHandle == NULL)
    {
        xTaskCreate(&State_Machine::logTask, "Starting log Task", 4096, this, PRIORITY_MEDIUM, &m_logTaskHandle);
    }
}

STATES State_Machine::getCurrentState()
{
    STATES currState;

    {
        SemaphoreGuard guard(m_stateMutex);
        if (guard.acquired())
        {
            currState = m_currState;
        }
    }

    return currState;
}

void State_Machine::idleSeq()
{
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGI("State_Machine", "Idle Sequence!");

    unsigned long startTime = millis();

    // if angle is out of bounds, we will disable active control; stay here and monitor angle
    while (!m_control.getControllable())
    {
        vTaskDelay(pdMS_TO_TICKS(200));

        // if here for more than 1 minute, enter light sleep. !!! Currently 6 seconds
        if (millis() - startTime > sleepTimeout_ms)
        {
            if (m_devices.canSleep())
            {
                SemaphoreGuard guard(m_stateMutex);
                if (guard.acquired())
                {
                    m_currState = LIGHT_SLEEP;
                }
                return;
            }
            else
            {
                startTime = millis(); // reset the timer
            }
        }
    }

    {
        SemaphoreGuard guard(m_stateMutex);
        if (guard.acquired())
        {
            m_currState = CONTROL;
        }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
}

// void State_Machine::checkControllableTask(void *pvParameters)
// {

//     vTaskDelay(pdMS_TO_TICKS(100));
//     // Convert generic pointer back to State_Machine*
//     auto *machine = static_cast<State_Machine *>(pvParameters);

//     while (true)
//     {
//         SemaphoreGuard guard(machine->m_controllableMutex);
//         if (guard.acquired())
//         {
//             machine->m_controllable = machine->m_control.controllableAngle();
//         }
//     }
// }

// // Setup CPU Usage Monitoring
// void State_Machine::cpuUsageMonitorSeq()
// {
//     if (m_cpuUsageTaskHandle == NULL)
//     {
//         xTaskCreate(&RTOS_Monitor::cpuUsageTask, "CPUUsageTask", 2048, this, 1, &m_cpuUsageTaskHandle);
//     }
// }