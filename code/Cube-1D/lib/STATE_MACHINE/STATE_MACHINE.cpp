#include "STATE_MACHINE.hpp"

STATE_MACHINE::STATE_MACHINE() : m_control(m_devices)
{
    m_stateMutex = xSemaphoreCreateMutex();
}

STATE_MACHINE::~STATE_MACHINE()
{
}

void STATE_MACHINE::begin()
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
    }
}

void STATE_MACHINE::loop()
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

void STATE_MACHINE::BLDCTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    machine->m_devices.m_bldc.enableMotor(true); // Enable motor

    while (machine->m_control.controllableAngle())
    {
        machine->m_control.updateBLDC();
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

    ESP_LOGI("STATE_MACHINE", "Ending BLDC Task!");

    // Delete the task explicitly
    machine->m_BLDCTaskHandle = NULL;
    vTaskDelete(NULL);
}

void STATE_MACHINE::balanceTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    while (true)
    {
        machine->m_control.updateBalanceControl();
        vTaskDelay(pdMS_TO_TICKS(balance_dt_ms));
    }
}

void STATE_MACHINE::updateFiltersTask(void *pvParameters)
{
    ESP_LOGI("STATE_MACHINE", "Starting Filters Task");

    vTaskDelay(pdMS_TO_TICKS(500));
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    while (true)
    {
        machine->m_control.updateFilters();
        vTaskDelay(pdMS_TO_TICKS(aquisitionFreq));
    }
}

void STATE_MACHINE::indicationTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

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

        vTaskDelay(pdMS_TO_TICKS(indication_dt_ms));
    }
}

void STATE_MACHINE::refreshStatusTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(refreshStatus_dt_ms));
        machine->m_devices.refreshStatusAll();
    }
}

void STATE_MACHINE::logTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    machine->m_devices.m_logger.startNewLog();

    while (true)
    {
        machine->m_devices.m_logger.logData();
        vTaskDelay(pdMS_TO_TICKS(log_dt_ms));
    }
}

void STATE_MACHINE::initialisationSeq()
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
        xTaskCreate(&STATE_MACHINE::indicationTask, "Indication Loop Task", 2048, this, PRIORITY_LOW, &m_indicationLoopTaskHandle);
    }
    if (m_refreshStatusTaskHandle == NULL)
    {
        xTaskCreate(&STATE_MACHINE::refreshStatusTask, "Device status check loop Task", 2048, this, PRIORITY_LOW, &m_refreshStatusTaskHandle);
    }
}

void STATE_MACHINE::criticalErrorSeq()
{
    ESP_LOGE("STATE_MACHINE CRITICAL ERROR", "Critical Error Sequence!");

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

void STATE_MACHINE::calibrationSeq()
{
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI("STATE_MACHINE CALIBRATION", "Calibration Sequence!");

    bool calibrated = m_devices.calibrateSeq();

    vTaskDelay(pdMS_TO_TICKS(100));

    if (m_updateFiltersTaskHandle == NULL)
    {
        m_control.setup();
        xTaskCreate(&STATE_MACHINE::updateFiltersTask, "Starting Filters Task", 4096, this, PRIORITY_HIGH, &m_updateFiltersTaskHandle);
    }

    SemaphoreGuard guard(m_stateMutex);
    if (guard.acquired())
    {
        m_currState = (calibrated) ? IDLE : CRITICAL_ERROR;
    }
}

bool STATE_MACHINE::canSleep()
{
    uint8_t deviceStatus = m_devices.getStatus();

    // If we don't have HV supply, we are connected to a PC and sleep is not possible via USB OTG if we want to maintain communication
    if ((deviceStatus & USBPD_BIT) == USBPD_BIT)
    {
        // check if IMU is enabled
        if ((deviceStatus & IMU_BIT) == IMU_BIT)
        {
            ESP_LOGI("STATE_MACHINE", "Can sleep!");
            return true;
        }
        else
        {
            ESP_LOGI("STATE_MACHINE", "IMU not enabled, cannot enter light sleep!");
            return false;
        }
    }
    else
    {
        ESP_LOGI("STATE_MACHINE", "USB connected, cannot enter light sleep!");
        return false;
    }
}

void STATE_MACHINE::lightSleepSeq()
{
    if (canSleep())
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
            ESP_LOGI("STATE_MACHINE", "Entering Light Sleep!");
            // Enter Light Sleep
            esp_light_sleep_start();

            ESP_LOGI("STATE_MACHINE", "Waking up from light sleep!");
            m_devices.wakeMode();
        }
    }

    SemaphoreGuard guard(m_stateMutex);
    if (guard.acquired())
    {
        m_currState = CONTROL;
    }
}

void STATE_MACHINE::controlSeq()
{
    if (m_logTaskHandle == NULL)
    {
        xTaskCreate(&STATE_MACHINE::logTask, "Starting log Task", 4096, this, PRIORITY_MEDIUM, &m_logTaskHandle);
    }

    if (m_balanceTaskHandle == NULL)
    {
        ESP_LOGI("STATE_MACHINE", "Starting balance Task");
        xTaskCreate(&STATE_MACHINE::balanceTask, "Starting balance Task", 4096, this, PRIORITY_HIGH, &m_balanceTaskHandle);
    }

    if (m_BLDCTaskHandle == NULL)
    {
        ESP_LOGI("STATE_MACHINE", "Starting BLDC Task");
        xTaskCreate(&STATE_MACHINE::BLDCTask, "Starting BLDC Task", 4096, this, PRIORITY_HIGH, &m_BLDCTaskHandle);
    }
}

STATES STATE_MACHINE::getCurrentState()
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

void STATE_MACHINE::idleSeq()
{
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGI("STATE_MACHINE", "Idle Sequence!");

    unsigned long startTime = millis();

    // if angle is out of bounds, we will disable active control; stay here and monitor angle
    while (!m_control.controllableAngle())
    {
        vTaskDelay(pdMS_TO_TICKS(200));

        // if here for more than 1 minute, enter light sleep. !!! Currently 6 seconds
        if (millis() - startTime > 6000)
        {
            if (canSleep())
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
