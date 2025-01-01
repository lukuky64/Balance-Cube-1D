#include "DEVICES.hpp"

DEVICES::DEVICES()
{
    m_statusMaskMutex = xSemaphoreCreateMutex();
}

// bool DEVICES::setupIndication(bool silentIndication)
// {
//     return true;
// }

DEVICES::~DEVICES()
{
    // disable all devices
}

bool DEVICES::setupUSBPD(uint8_t SCL, uint8_t SDA)
{
    return true;
}

bool DEVICES::setupBLDC()
{
    return true;
}

bool DEVICES::setupIMU(uint8_t CS, uint8_t MISO, uint8_t MOSI, uint8_t CLK)
{
    return true;
}

bool DEVICES::setupROT_ENC()
{
    return true;
}

bool DEVICES::setupMAG(uint8_t CS, uint8_t MISO, uint8_t MOSI, uint8_t CLK)
{
    return true;
}

bool DEVICES::setupSerialLog()
{
    return true;
}

bool DEVICES::setupSDLog(uint8_t CS, uint8_t MISO, uint8_t MOSI, uint8_t CLK)
{
    return true;
}

bool DEVICES::setupServo(uint8_t servoPin)
{
    return true;
}

void DEVICES::indicateStatus()
{
    if (!checkRequirements())
    {
        m_indicators.showCriticalError();
    }
    else if (m_statusMask != m_prefMask)
    {
        m_indicators.showWarning();
    }
    else
    {
        m_indicators.showAllGood();
    }
}

void DEVICES::checkStatusAll()
{
    ESP_LOGI("DEVICES", "Checking all device statuses...");

    // Take mutex before modifying m_statusMask
    if (xSemaphoreTake(m_statusMaskMutex, portMAX_DELAY) == pdTRUE)
    {
        uint8_t statusMask = 0;
        statusMask |= (INDICATION_BIT && m_indicators.checkStatus());
        statusMask |= (USBPD_BIT && m_usbPD.checkStatus());
        statusMask |= (BLDC_BIT && m_bldc.checkStatus());
        statusMask |= (IMU_BIT && m_imu.checkStatus());
        statusMask |= (ROT_ENC_BIT && m_rotEnc.checkStatus());
        statusMask |= (MAG_BIT && m_magEnc.checkStatus());
        statusMask |= (SERIAL_BIT && m_logger.m_serialTalker.checkStatus());
        statusMask |= (SD_BIT && m_logger.m_sdTalker.checkStatus());
        statusMask |= (SERVO_BIT && m_servo.checkStatus());
        m_statusMask = statusMask; // this ensure a single write operation so we might not need mutex here

        xSemaphoreGive(m_statusMaskMutex);
    }
}

bool DEVICES::initialisationSeq(bool logSD, bool logSerial, bool SilentIndication, bool servoBraking, bool useIMU, bool useROT_ENC)
{
    m_statusMask = 0;
    m_prefMask = 0;

    // set up indication. If silent indication is enabled, only set up RGB LED
    {
        m_indicators.setupRGBLed(LED_R, LED_G, LED_B);

        if (!SilentIndication)
        {
            m_indicators.setupBuzzer(BUZZER);
        }

        m_statusMask |= INDICATION_BIT;
        m_prefMask |= INDICATION_BIT;
    }

    // set up Servo
    if (servoBraking)
    {
        m_prefMask |= SERVO_BIT;

        if (setupServo(SERVO))
        {
            m_statusMask |= SERVO_BIT;
        }
    }

    // set up IMU
    if (useIMU)
    {
        m_prefMask |= IMU_BIT;

        if (setupIMU(SPI_CS_IMU, SPI_MISO, SPI_MOSI, SPI_CLK))
        {
            m_statusMask |= IMU_BIT;
        }
    }

    // set up ROT_ENC
    if (useROT_ENC)
    {
        m_prefMask |= ROT_ENC_BIT;

        if (setupROT_ENC())
        {
            m_statusMask |= ROT_ENC_BIT;
        }
    }

    // set up Serial logging
    if (logSerial)
    {
        m_prefMask |= SERIAL_BIT;

        if (setupSerialLog())
        {
            m_statusMask |= SERIAL_BIT;
        }
    }

    // set up SD logging
    if (logSD)
    {
        m_prefMask |= SD_BIT;

        if (setupSDLog(SPI_CS_SD, SPI_MISO_SD, SPI_MOSI_SD, SPI_CLK_SD))
        {
            m_statusMask |= SD_BIT;
        }
    }

    // next few are mandatory

    m_prefMask |= USBPD_BIT | MAG_BIT | BLDC_BIT;

    // set up voltage
    if (setupUSBPD(I2C_SCL, I2C_SDA))
    {
        m_statusMask |= USBPD_BIT;
    }

    // set up Magnetic sensor for BLDC
    if (setupMAG(SPI_CS_MAG, SPI_MISO, SPI_MOSI, SPI_CLK))
    {
        m_statusMask |= MAG_BIT;
    }

    // set up BLDC
    if (setupBLDC())
    {
        m_statusMask |= BLDC_BIT;
    }

    // explicitly check each required bit

    if (checkRequirements())
    {
        ESP_LOGI("Initialisation", "Minimum device successes satisifed!");
        m_indicators.showSuccess();
        return true;
    }
    else
    {
        ESP_LOGE("Initialisation", "Minimum device successes not satisifed.");
        return false;
    }
}

bool DEVICES::checkRequirements()
{
    // Take mutex before modifying m_statusMask
    if (xSemaphoreTake(m_statusMaskMutex, portMAX_DELAY) == pdTRUE)
    {
        const bool isIndicationOk = (m_statusMask & INDICATION_BIT) != 0;
        const bool isUsbpdOk = (m_statusMask & USBPD_BIT) != 0;
        const bool isBldcOk = (m_statusMask & BLDC_BIT) != 0;
        const bool isImuOrEncOk = ((m_statusMask & IMU_BIT) != 0) || ((m_statusMask & ROT_ENC_BIT) != 0);

        xSemaphoreGive(m_statusMaskMutex);
        return isIndicationOk && isUsbpdOk && isBldcOk && isImuOrEncOk;
    }
    else
    {
        return false;
    }
}