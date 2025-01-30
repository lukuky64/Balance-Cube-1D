#include "State_Machine.hpp"
#include "Params.hpp"
// #include "perfmon.hpp"
#include "WebSocketServer.hpp"

WebSocketServer wsServer;

State_Machine *State_Machine::instance = nullptr; // Initialize static pointer. Was being used for USBSerial reading stuff

State_Machine::State_Machine() : m_control(m_devices)
{
    m_stateMutex = xSemaphoreCreateMutex();
}

State_Machine::~State_Machine()
{
}

void State_Machine::begin()
{
    // perfmon_start();

    {
        SemaphoreGuard guard(m_stateMutex);
        if (guard.acquired())
        {
            m_currState = INITIALISATION;
        }
    }

    if (m_taskManagerTaskHandle == NULL)
    {
        ESP_LOGI(TAG, "Starting Task Manager Task");
        // xTaskCreate(&State_Machine::taskManagerTask, "Starting Task Manager", 4096, this, PRIORITY_MEDIUM, &m_taskManagerTaskHandle);
        xTaskCreatePinnedToCore(&State_Machine::taskManagerTask, "Starting Task Manager", 4096, this, PRIORITY_MEDIUM, &m_taskManagerTaskHandle, 1);
    }
}

void State_Machine::taskManagerTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    while (true)
    {
        machine->loop();

        // ESP_LOGI(TAG, "Current state: %s", machine->stateToString(static_cast<STATES>(machine->getCurrentState())));

        // UBaseType_t uxHighWaterMark;
        // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        // ESP_LOGI(TAG, "Remaining stack: %u words\n", uxHighWaterMark);

        // machine->printCpuUsage();

        // UBaseType_t stackRemaining = uxTaskGetStackHighWaterMark(machine->m_wifiTaskHandle);
        // ESP_LOGI(TAG, "Remaining stack for WiFi Task: %u words\n", stackRemaining);

        // stackRemaining = uxTaskGetStackHighWaterMark(machine->m_logTaskHandle);
        // ESP_LOGI(TAG, "Remaining stack for Log Task: %u words\n", stackRemaining);

        // stackRemaining = uxTaskGetStackHighWaterMark(machine->m_BLDCTaskHandle);
        // ESP_LOGI(TAG, "Remaining stack for BLDC Task: %u words\n", stackRemaining);

        // stackRemaining = uxTaskGetStackHighWaterMark(machine->m_balanceTaskHandle);
        // ESP_LOGI(TAG, "Remaining stack for Balance Task: %u words\n", stackRemaining);

        // stackRemaining = uxTaskGetStackHighWaterMark(machine->m_updateFiltersTaskHandle);
        // ESP_LOGI(TAG, "Remaining stack for Filters Task: %u words\n", stackRemaining);

        // stackRemaining = uxTaskGetStackHighWaterMark(machine->m_refreshStatusTaskHandle);
        // ESP_LOGI(TAG, "Remaining stack for Refresh Status Task: %u words\n", stackRemaining);

        // stackRemaining = uxTaskGetStackHighWaterMark(machine->m_indicationLoopTaskHandle);
        // ESP_LOGI(TAG, "Remaining stack for Indication Task: %u words\n", stackRemaining);

        vTaskDelay(pdMS_TO_TICKS(Params::TASK_MANAGER_MS)); // Loop current has blocking in certain functions
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
        logSeq();  // initial logging start
        wifiSeq(); // start wifi server
    }
    break;
    case IDLE:
    {
        idleSeq(); // this is a blocking function. fine for our case but not good for task manager
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

// void IRAM_ATTR State_Machine::onBLDCTimer()
// {
//     if (instance && instance->m_BLDCTaskHandle != NULL) // Use instance pointer
//     {
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//         vTaskNotifyGiveFromISR(instance->m_BLDCTaskHandle, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }

// void State_Machine::startBLDCTimer()
// {
//     bldcTimer = timerBegin(0, 80, true); // Timer 0, prescaler 80 → 1 tick = 1 µs
//     timerAttachInterrupt(bldcTimer, &State_Machine::onBLDCTimer, true);
//     timerAlarmWrite(bldcTimer, Params::BLDC_MS * 1000, true); // Set period in µs
//     timerAlarmEnable(bldcTimer);                              // Enable the timer
// }

void State_Machine::BLDCTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    machine->m_devices.m_bldc.enableMotor(true); // Enable motor

    TickType_t xLastWakeTime = xTaskGetTickCount();               // Store initial tick count
    const TickType_t xFrequency = pdMS_TO_TICKS(Params::BLDC_MS); // Task period

    while (machine->m_control.getControllable())
    {
        {
            // TimerGuard guard(TAG, "update BLDC task");
            machine->m_control.updateBLDC(); // currently only takes ~ 5uS or 200kHz
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
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

    ESP_LOGI(TAG, "Ending BLDC Task!");

    // Delete the task explicitly
    machine->m_BLDCTaskHandle = NULL;
    vTaskDelete(NULL);
}

void State_Machine::balanceTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    machine->m_control.setState();

    while (true)
    {
        machine->m_control.updateBalanceControl(Params::BALANCE_MS); // currently only takes ~ 30uS or 33kHz
        vTaskDelay(pdMS_TO_TICKS(Params::BALANCE_MS));
    }
}

void State_Machine::updateFiltersTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    while (true)
    {
        {
            // TimerGuard guard(TAG, "update filter task");
            machine->m_control.updateData(); // takes ~ 380uS or 2.63kHz
        }
        vTaskDelay(pdMS_TO_TICKS(Params::AQUISITION_MS));
    }
}

void State_Machine::indicationTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
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

        vTaskDelay(pdMS_TO_TICKS(Params::INDICATION_MS));
    }
}

void State_Machine::refreshStatusTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(Params::REFRESH_STATUS_MS)); // delay first to create initial offset
        machine->m_devices.refreshStatusAll();
    }
}

void State_Machine::logTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    ESP_LOGI(TAG, "Starting log Task");

    bool successLog = machine->m_devices.m_logger.startNewLog();

    if (successLog)
    {
        // This method is more precise than vTaskDelay
        TickType_t xLastWakeTime = xTaskGetTickCount();              // Get initial tick count
        const TickType_t xFrequency = pdMS_TO_TICKS(Params::LOG_MS); // Logging period\


        while (true)
        {
            machine->m_devices.m_logger.logData(machine->m_control.getDataBuffer(), Params::LOG_COLUMNS); // around 160 microseconds without flush (3 float points and time), ~37ms for flush (4kB)
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }
    else
    {
        machine->m_logTaskHandle = NULL; // clear the handle
        vTaskDelete(NULL);
    }
}

void State_Machine::wifiTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(50));
    // Convert generic pointer back to State_Machine*
    auto *machine = static_cast<State_Machine *>(pvParameters);

    wsServer.begin();

    while (true)
    {
        wsServer.loop();                                                                                                                                                                        // Process WebSocket events
        String JSONmessage = "{\"angle\":" + String(machine->m_control.m_filters.filter_theta.getValue()) + ",\"omega\":" + String(machine->m_control.m_filters.filter_omega.getValue()) + "}"; // JSON
        wsServer.sendMessage(JSONmessage);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    machine->m_wifiTaskHandle = NULL; // clear the handle
    vTaskDelete(NULL);
}

// void State_Machine::ControlabilityTask(void *pvParameters)
// {
//     vTaskDelay(pdMS_TO_TICKS(50));
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
    STATES currState = m_devices.init(Params::LOG_SD, Params::LOG_SERIAL, Params::SILENT_INDICATION, Params::SERVO_BRAKING, Params::USE_IMU, Params::USE_ROT_ENC) ? CALIBRATION : CRITICAL_ERROR;

    {
        SemaphoreGuard guard(m_stateMutex);
        if (guard.acquired())
        {
            m_currState = currState;
        }
    }

    if (m_indicationLoopTaskHandle == NULL)
    {
        // xTaskCreate(&State_Machine::indicationTask, "Indication Loop Task", 2048, this, PRIORITY_LOW, &m_indicationLoopTaskHandle); // uses 1836 bytes of stack
        xTaskCreatePinnedToCore(&State_Machine::indicationTask, "Indication Loop Task", 4096, this, PRIORITY_LOW, &m_indicationLoopTaskHandle, 1);
    }
    if (m_refreshStatusTaskHandle == NULL)
    {
        // xTaskCreate(&State_Machine::refreshStatusTask, "Device status check loop Task", 2048, this, PRIORITY_LOW, &m_refreshStatusTaskHandle);
        xTaskCreatePinnedToCore(&State_Machine::refreshStatusTask, "Device status check loop Task", 4096, this, PRIORITY_LOW, &m_refreshStatusTaskHandle, 1);
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
    vTaskDelay(pdMS_TO_TICKS(50));

    ESP_LOGI("State_Machine CALIBRATION", "Calibration Sequence!");

    bool succ = m_devices.calibrateSeq();

    succ &= m_control.setup();

    if ((m_updateFiltersTaskHandle == NULL) && succ)
    {
        // xTaskCreate(&State_Machine::updateFiltersTask, "Starting Filters Task", 4096, this, PRIORITY_HIGH, &m_updateFiltersTaskHandle);
        xTaskCreatePinnedToCore(&State_Machine::updateFiltersTask, "Starting Filters Task", 4096, this, PRIORITY_HIGH, &m_updateFiltersTaskHandle, 1);
    }

    SemaphoreGuard guard(m_stateMutex);
    if (guard.acquired())
    {
        m_currState = (succ) ? IDLE : CRITICAL_ERROR;
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
        ESP_LOGI(TAG, "Entering Light Sleep!");
        esp_light_sleep_start(); // Enter Light Sleep

        ESP_LOGI(TAG, "Waking up from light sleep!");
        m_devices.wakeMode();
    }

    SemaphoreGuard guard(m_stateMutex);
    if (guard.acquired())
    {
        m_currState = CONTROL;
    }
}

void State_Machine::wifiSeq()
{
    if (m_wifiTaskHandle == NULL)
    {
        xTaskCreatePinnedToCore(&State_Machine::wifiTask, "WiFi Task", 4096, this, PRIORITY_HIGH, &m_wifiTaskHandle, 0); // core 0 for wifi tasks
    }
}

void State_Machine::controlSeq()
{
    // ESP_LOGI(TAG, "Control Sequence!");

    if (m_balanceTaskHandle == NULL)
    {
        ESP_LOGI(TAG, "Starting balance Task");
        // xTaskCreate(&State_Machine::balanceTask, "balance Task", 4096, this, PRIORITY_HIGH, &m_balanceTaskHandle);
        xTaskCreatePinnedToCore(&State_Machine::balanceTask, "balance Task", 4096, this, PRIORITY_HIGH, &m_balanceTaskHandle, 1); // Core 1 for real-time tasks
    }

    if (m_BLDCTaskHandle == NULL)
    {
        ESP_LOGI(TAG, "Starting BLDC Task");
        // xTaskCreate(&State_Machine::BLDCTask, "Starting BLDC Task", 4096, this, PRIORITY_HIGH, &m_BLDCTaskHandle);
        xTaskCreatePinnedToCore(&State_Machine::BLDCTask, "BLDC Task", 4096, this, PRIORITY_HIGH, &m_BLDCTaskHandle, 1); // Core 1 for real-time tasks
    }
}

void State_Machine::logSeq()
{
    if (m_logTaskHandle == NULL)
    {
        // xTaskCreate(&State_Machine::logTask, "Starting log Task", 4096, this, PRIORITY_MEDIUM, &m_logTaskHandle);
        xTaskCreatePinnedToCore(&State_Machine::logTask, "Starting log Task", 4096, this, PRIORITY_MEDIUM, &m_logTaskHandle, 1);
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
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP_LOGI(TAG, "Idle Sequence!");

    unsigned long startTime = millis();

    // if angle is out of bounds, we will disable active control; stay here and monitor angle
    while (!m_control.getControllable())
    {
        vTaskDelay(pdMS_TO_TICKS(50));

        if (millis() - startTime > Params::SLP_TIMEOUT_MS)
        {
            if (Params::ALLOW_SLEEP == 1) // m_devices.canSleep()
            {
                ESP_LOGI(TAG, "Sleep is allowed!");
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

    vTaskDelay(pdMS_TO_TICKS(50));
}

// void State_Machine::checkControllableTask(void *pvParameters)
// {

//     vTaskDelay(pdMS_TO_TICKS(50));
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

const char *State_Machine::stateToString(STATES state)
{
    switch (state)
    {
    case INITIALISATION:
        return "INITIALISATION";
    case CRITICAL_ERROR:
        return "CRITICAL_ERROR";
    case CALIBRATION:
        return "CALIBRATION";
    case IDLE:
        return "IDLE";
    case LIGHT_SLEEP:
        return "LIGHT_SLEEP";
    case CONTROL:
        return "CONTROL";
    default:
        return "UNKNOWN_STATE";
    }
}