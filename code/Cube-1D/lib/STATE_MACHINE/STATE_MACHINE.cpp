#include "STATE_MACHINE.hpp"

STATE_MACHINE::STATE_MACHINE()
{
}

STATE_MACHINE::~STATE_MACHINE()
{
}

void STATE_MACHINE::setup()
{
}

void STATE_MACHINE::begin()
{
    delay(3000);
    // Setup the initial state
    m_currState = INITIALISATION;

    while (true)
    {
        loop();
    }
}

void STATE_MACHINE::loop()
{
    switch (m_currState)
    {
    case INITIALISATION:
    {
        m_currState = m_devices.initialisationSeq(LOG_SD, LOG_SERIAL, SILENT_INDICATION, SERVO_BRAKING, USE_IMU, USE_ROT_ENC)
                          ? CALIBRATION
                          : CRITICAL_ERROR;

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
    case LOW_POWER_IDLE:
    {
        lowPowerIdleSeq();
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
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    while (true)
    {
        bool requirementsMet = machine->m_devices.indicateStatus();

        if (!requirementsMet)
        {
            machine->m_currState = CRITICAL_ERROR;
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Indicate status every 2 seconds
    }
}

void STATE_MACHINE::refreshStatusTask(void *pvParameters)
{
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(10000)); // Check every 10 seconds
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

    m_currState = INITIALISATION;
}

void STATE_MACHINE::calibrationSeq()
{
    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_LOGI("STATE_MACHINE CALIBRATION", "Calibration Sequence!");

    vTaskDelay(pdMS_TO_TICKS(5000));
    m_devices.setStatus(0);
    vTaskDelay(pdMS_TO_TICKS(5000));

    // while (true)
    // {
    //     // m_devices.m_indicators.showWarning();
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
}

void STATE_MACHINE::lowPowerIdleSeq()
{
    // ESP_LOGI("STATE_MACHINE LOW POWER IDLE", "Low Power Idle Sequence!");

    // while (true)
    // {
    //     m_devices.m_indicators.showAllGood();
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
}

void STATE_MACHINE::controlSeq()
{
    // ESP_LOGI("STATE_MACHINE CONTROL", "Control Sequence!");

    // while (true)
    // {
    //     m_devices.m_indicators.showSuccess();
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
}

STATES STATE_MACHINE::getCurrentState()
{
    return m_currState;
}