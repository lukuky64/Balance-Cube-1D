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
        bool initSuccess = m_devices.initialisationSeq(LOG_SD, LOG_SERIAL, SILENT_INDICATION, SERVO_BRAKING, USE_IMU, USE_ROT_ENC);

        ESP_LOGI("STATE_MACHINE INITIALISATION", "Starting Indication Loop Task");
        xTaskCreate(&STATE_MACHINE::indicationLoopTask, "Indication Loop Task", 2048, this, PRIORITY_MEDIUM, nullptr);
        xTaskCreate(&STATE_MACHINE::checkStatusTask, "Device status check loop Task", 2048, this, PRIORITY_MEDIUM, nullptr);

        if (initSuccess)
        {
            m_currState = CALIBRATION;
        }
        else
        {
            m_currState = CRITICAL_ERROR;
        }
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
        machine->m_devices.indicateStatus();

        // UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(NULL);
        // ESP_LOGI("INDICATION LOOP TASK", "High Water Mark: %d", highWaterMark);

        vTaskDelay(pdMS_TO_TICKS(2000)); // Indicate status every 2 seconds
    }
}

void STATE_MACHINE::checkStatusTask(void *pvParameters)
{
    // Convert generic pointer back to STATE_MACHINE*
    auto *machine = static_cast<STATE_MACHINE *>(pvParameters);

    while (true)
    {
        machine->m_devices.checkStatusAll();
        vTaskDelay(pdMS_TO_TICKS(5000)); // Check every 5 seconds
    }
}

void STATE_MACHINE::criticalErrorSeq()
{
    // ESP_LOGE("STATE_MACHINE CRITICAL ERROR", "Critical Error Sequence!");

    // while (true)
    // {
    //     m_devices.m_indicators.showCriticalError();
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
}

void STATE_MACHINE::calibrationSeq()
{
    m_currState = LOW_POWER_IDLE;

    // ESP_LOGI("STATE_MACHINE CALIBRATION", "Calibration Sequence!");

    // while (true)
    // {
    //     m_devices.m_indicators.showWarning();
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