#include "stateMachine.hpp"

STATEMACHINE::STATEMACHINE()
{
}

STATEMACHINE::~STATEMACHINE()
{
}

void STATEMACHINE::setup(bool logSD, bool logSerial, bool SilentIndication, bool servoBraking, bool useIMU, bool useROT_ENC)
{
    m_currPref.logSD = logSD;
    m_currPref.logSerial = logSerial;
    m_currPref.SilentIndication = SilentIndication;
    m_currPref.servoBraking = servoBraking;
    m_currPref.useIMU = useIMU;
    m_currPref.useROT_ENC = useROT_ENC;
}

void STATEMACHINE::begin()
{
    // Setup the initial state
    m_currState = INITIALISATION;
}

void STATEMACHINE::loop()
{
    switch (m_currState)
    {
    case INITIALISATION:
        initialisationTask();
        break;
    case CRITICAL_ERROR:
        criticalErrorTask();
        break;
    case CALIBRATION:
        calibrationTask();
        break;
    case LOW_POWER_IDLE:
        lowPowerIdleTask();
        break;
    case CONTROL:
        controlTask();
        break;
    default:
        break;
    }
}

void STATEMACHINE::initialisationTask()
{
    uint8_t successMask = 0;

    // set up indication
    if (m_currPref.SilentIndication)
    {
        if (m_devices.setupIndication(m_currPref.SilentIndication))
        {
            successMask |= INDICATION_BIT;
        }
    }

    // set up Servo
    if (m_currPref.servoBraking)
    {
        if (m_devices.setupServo())
        {
            successMask |= SERVO_BIT;
        }
    }

    // set up IMU
    if (m_currPref.useIMU)
    {
        if (m_devices.setupIMU())
        {
            successMask |= IMU_BIT;
        }
    }

    // set up ROT_ENC
    if (m_currPref.useROT_ENC)
    {
        if (m_devices.setupROT_ENC())
        {
            successMask |= ROT_ENC_BIT;
        }
    }

    // set up Serial logging
    if (m_currPref.logSerial)
    {
        if (m_devices.setupSerialLog())
        {
            successMask |= SERIAL_BIT;
        }
    }

    // set up SD logging
    if (m_currPref.logSD)
    {
        if (m_devices.setupSDLog())
        {
            successMask |= SD_BIT;
        }
    }

    // next few are mandatory

    // set up voltage
    if (m_devices.setupVoltage())
    {
        successMask |= VOLTAGE_BIT;
    }

    // set up Magnetic sensor for BLDC
    if (m_devices.setupMAG())
    {
        successMask |= MAG_BIT;
    }

    // set up BLDC
    if (m_devices.setupBLDC())
    {
        successMask |= BLDC_BIT;
    }

    if ((successMask && m_requiredMask) != m_requiredMask)
    {
        ESP_LOGI("Initialisation", "Minimum device successes not satisifed.");
    }
    else
    {
        ESP_LOGI("Initialisation", "Minimum device successes satisifed!");
        m_currState = CALIBRATION;
    }
}