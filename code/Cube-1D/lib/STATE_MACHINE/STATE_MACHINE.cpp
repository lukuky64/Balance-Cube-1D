#include "STATE_MACHINE.hpp"

STATE_MACHINE::STATE_MACHINE() : m_devices(), m_control(m_devices)
{
    m_refreshStatusPeriod = 5000; // 5 seconds
    m_indicationPeriod = 2000;    // 2 seconds
}

STATE_MACHINE::~STATE_MACHINE()
{
}

void STATE_MACHINE::setup()
{
}

void STATE_MACHINE::begin()
{
    m_stateMutex = xSemaphoreCreateMutex();
    delay(3000);
    // Setup the initial state

    if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
    {
        m_currState = INITIALISATION;
        xSemaphoreGive(m_stateMutex);
    }

    while (true)
    {
        loop();
    }
}

void STATE_MACHINE::loop()
{
    STATES currState;
    if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
    {
        currState = m_currState;
        xSemaphoreGive(m_stateMutex);
    }

    switch (currState)
    {
    case INITIALISATION:
    {
        if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
        {
            m_currState = m_devices.initialisationSeq(LOG_SD, LOG_SERIAL, SILENT_INDICATION, SERVO_BRAKING, USE_IMU, USE_ROT_ENC) ? CALIBRATION : CRITICAL_ERROR;
            xSemaphoreGive(m_stateMutex);
        }

        xTaskCreate(&STATE_MACHINE::indicationLoopTask, "Indication Loop Task", 2048, this, PRIORITY_MEDIUM, &m_indicationLoopTaskHandle);
        xTaskCreate(&STATE_MACHINE::refreshStatusTask, "Device status check loop Task", 2048, this, PRIORITY_MEDIUM, &m_refreshStatusTaskHandle);
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

void STATE_MACHINE::indicationLoopTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(500));
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    while (true)
    {
        bool requirementsMet = machine->m_devices.indicateStatus();

        if (!requirementsMet)
        {
            if (xSemaphoreTake(machine->m_stateMutex, portMAX_DELAY))
            {
                machine->m_currState = CRITICAL_ERROR;
                xSemaphoreGive(machine->m_stateMutex);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(machine->m_indicationPeriod)); // Indicate status every 2 seconds
    }
}

void STATE_MACHINE::refreshStatusTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(500));
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS((machine->m_refreshStatusPeriod))); // Check every 5 seconds
        machine->m_devices.refreshStatusAll();
    }
}

void STATE_MACHINE::criticalErrorSeq()
{
    ESP_LOGE("STATE_MACHINE CRITICAL ERROR", "Critical Error Sequence!");

    // destroy task
    vTaskDelete(m_refreshStatusTaskHandle);
    m_refreshStatusTaskHandle = nullptr; // clear the handle

    vTaskDelay(pdMS_TO_TICKS(5000)); // wait for 5 seconds before restarting the initialisation sequence

    // destroy task
    vTaskDelete(m_indicationLoopTaskHandle);
    m_indicationLoopTaskHandle = nullptr; // clear the handle

    if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
    {
        m_currState = INITIALISATION;
        xSemaphoreGive(m_stateMutex);
    }
}

void STATE_MACHINE::calibrationSeq()
{
    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_LOGI("STATE_MACHINE CALIBRATION", "Calibration Sequence!");

    if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
    {
        m_currState = m_devices.calibrateSeq() ? IDLE : CRITICAL_ERROR;
        xSemaphoreGive(m_stateMutex);
    }
}

void STATE_MACHINE::lightSleepSeq()
{
    uint8_t deviceStatus = m_devices.getStatus();

    // If we don't have HV supply, we are connected to a PC and sleep is not possible via USB OTG if we want to maintain communication
    if (deviceStatus & USBPD_BIT != USBPD_BIT)
    {
        ESP_LOGI("STATE_MACHINE", "Light sleep Sequence!");

        // check if IMU is enabled
        if (deviceStatus & IMU_BIT != 0)
        {
            if (m_devices.sleepMode())
            {
                // Enter Light Sleep
                esp_light_sleep_start();

                m_devices.wakeMode();
            }
        }
        else
        {
            ESP_LOGI("STATE_MACHINE", "IMU not enabled, cannot enter light sleep!");
        }
    }
    else
    {
        ESP_LOGI("STATE_MACHINE", "USB connected, cannot enter light sleep!");
    }

    if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
    {
        m_currState = IDLE;
        xSemaphoreGive(m_stateMutex);
    }
}

void STATE_MACHINE::controlSeq()
{
    ESP_LOGI("STATE_MACHINE CONTROL", "Control Sequence!");

    while (m_control.controlableAngle())
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

STATES STATE_MACHINE::getCurrentState()
{
    STATES currState;

    if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
    {
        currState = m_currState;
        xSemaphoreGive(m_stateMutex);
    }

    return currState;
}

void STATE_MACHINE::idleSeq()
{
    ESP_LOGI("STATE_MACHINE", "Idle Sequence!");

    unsigned long startTime = millis();

    // if angle is out of bounds, we will disable active control; stay here and monitor angle
    while (!m_control.controlableAngle())
    {
        vTaskDelay(pdMS_TO_TICKS(200));

        // if here for more than 1 minute, enter light sleep. !!! Currently 6 seconds
        if (millis() - startTime > 6000)
        {
            if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
            {
                m_currState = LIGHT_SLEEP;
                xSemaphoreGive(m_stateMutex);
            }
            break;
        }
    }

    // if we are here, we can control the angle
    if (xSemaphoreTake(m_stateMutex, portMAX_DELAY))
    {
        m_currState = CONTROL;
        xSemaphoreGive(m_stateMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
}
