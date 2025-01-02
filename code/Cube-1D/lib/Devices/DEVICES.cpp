#include "DEVICES.hpp"

DEVICES::DEVICES()
{
    m_statusMaskMutex = xSemaphoreCreateMutex();
    m_prefMaskMutex = xSemaphoreCreateMutex();
}

// bool DEVICES::setupIndication(bool silentIndication)
// {
//     return true;
// }

DEVICES::~DEVICES()
{
    // disable all devices
}

bool DEVICES::setupUSBPD(gpio_num_t SCL, gpio_num_t SDA)
{
    return false;
}

bool DEVICES::setupBLDC()
{
    return true;
}

bool DEVICES::setupIMU(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK, gpio_num_t intPin)
{
    return true;
}

bool DEVICES::setupROT_ENC()
{
    return true;
}

bool DEVICES::setupMAG(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK)
{
    return true;
}

bool DEVICES::setupSerialLog()
{
    return true;
}

bool DEVICES::setupSDLog(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK)
{
    return true;
}

bool DEVICES::setupServo(gpio_num_t servoPin)
{
    return true;
}

bool DEVICES::indicateStatus()
{
    bool requirementsMet = checkRequirementsMet();

    uint8_t statusMask = 0;
    if (xSemaphoreTake(m_statusMaskMutex, portMAX_DELAY))
    {
        statusMask = m_statusMask;
        xSemaphoreGive(m_statusMaskMutex);
    }

    uint8_t prefMask = 0;
    if (xSemaphoreTake(m_prefMaskMutex, portMAX_DELAY))
    {
        prefMask = m_prefMask;
        xSemaphoreGive(m_prefMaskMutex);
    }

    if (!requirementsMet)
    {
        m_indicators.showCriticalError();
    }
    else if (statusMask != prefMask)
    {
        ESP_LOGI("DEVICES", "Status: %d, Pref: %d", statusMask, prefMask);
        m_indicators.showWarning();
    }
    else
    {
        m_indicators.showAllGood();
    }

    return requirementsMet;
}

void DEVICES::refreshStatusAll()
{
    ESP_LOGI("DEVICES", "Checking all device statuses...");

    uint8_t statusMask = 0;
    statusMask |= (m_indicators.checkStatus() ? INDICATION_BIT : 0);
    statusMask |= (m_usbPD.checkStatus() ? USBPD_BIT : 0);
    statusMask |= (m_bldc.checkStatus() ? BLDC_BIT : 0);
    statusMask |= (m_imu.checkStatus() ? IMU_BIT : 0);
    statusMask |= (m_rotEnc.checkStatus() ? ROT_ENC_BIT : 0);
    statusMask |= (m_magEnc.checkStatus() ? MAG_BIT : 0);
    statusMask |= (m_logger.m_serialTalker.checkStatus() ? SERIAL_BIT : 0);
    statusMask |= (m_logger.m_sdTalker.checkStatus() ? SD_BIT : 0);
    statusMask |= (m_servo.checkStatus() ? SERVO_BIT : 0);

    if (xSemaphoreTake(m_statusMaskMutex, portMAX_DELAY))
    {
        uint8_t prefMask = 0;
        if (xSemaphoreTake(m_prefMaskMutex, portMAX_DELAY))
        {
            prefMask = m_prefMask;
            xSemaphoreGive(m_prefMaskMutex);
        }

        m_statusMask = statusMask & prefMask;
        xSemaphoreGive(m_statusMaskMutex);
    }
}

bool DEVICES::initialisationSeq(bool logSD, bool logSerial, bool SilentIndication, bool servoBraking, bool useIMU, bool useROT_ENC)
{
    vTaskDelay(pdMS_TO_TICKS(500));

    bool result = false;

    uint8_t statusMask = 0;
    uint8_t prefMask = 0;

    // set up indication. If silent indication is enabled, only set up RGB LED
    {
        m_indicators.setupRGBLed(LED_R, LED_G, LED_B);

        if (!SilentIndication)
        {
            m_indicators.setupBuzzer(BUZZER);
        }

        statusMask |= INDICATION_BIT;
        prefMask |= INDICATION_BIT;
    }

    // set up Servo
    if (servoBraking)
    {
        prefMask |= SERVO_BIT;

        if (setupServo(SERVO))
        {
            statusMask |= SERVO_BIT;
        }
    }

    // set up IMU
    if (useIMU)
    {
        prefMask |= IMU_BIT;

        if (setupIMU(SPI_CS_IMU, SPI_MISO, SPI_MOSI, SPI_CLK, IMU_INT1))
        {
            esp_sleep_enable_ext0_wakeup(m_imu.getIntPin(), 1); // 1 = High level wake-up using the IMU's interrupt pin

            statusMask |= IMU_BIT;
        }
    }

    // set up ROT_ENC
    if (useROT_ENC)
    {
        prefMask |= ROT_ENC_BIT;

        if (setupROT_ENC())
        {
            statusMask |= ROT_ENC_BIT;
        }
    }

    // set up Serial logging
    if (logSerial)
    {
        prefMask |= SERIAL_BIT;

        if (setupSerialLog())
        {
            statusMask |= SERIAL_BIT;
        }
    }

    // set up SD logging
    if (logSD)
    {
        prefMask |= SD_BIT;

        if (setupSDLog(SPI_CS_SD, SPI_MISO_SD, SPI_MOSI_SD, SPI_CLK_SD))
        {
            statusMask |= SD_BIT;
        }
    }

    // next few are mandatory

    // set up voltage
    if (setupUSBPD(I2C_SCL, I2C_SDA))
    {
        statusMask |= USBPD_BIT;
        prefMask |= USBPD_BIT;
    }

    // set up Magnetic sensor for BLDC
    if (setupMAG(SPI_CS_MAG, SPI_MISO, SPI_MOSI, SPI_CLK))
    {
        statusMask |= MAG_BIT;
        prefMask |= MAG_BIT;
    }

    // set up BLDC
    if (setupBLDC())
    {
        statusMask |= BLDC_BIT;
        prefMask |= BLDC_BIT;
    }

    if (xSemaphoreTake(m_statusMaskMutex, portMAX_DELAY))
    {
        m_statusMask = statusMask;
        xSemaphoreGive(m_statusMaskMutex);
    }

    if (xSemaphoreTake(m_prefMaskMutex, portMAX_DELAY))
    {
        m_prefMask = prefMask;
        xSemaphoreGive(m_prefMaskMutex);
    }

    // explicitly check each required bit
    if (checkRequirementsMet())
    {
        ESP_LOGI("Initialisation", "Minimum device successes satisfied!");
        m_indicators.showSuccess();
        result = true;
    }
    else
    {
        ESP_LOGE("Initialisation", "Minimum device successes not satisfied.");
        result = false;
    }

    vTaskDelay(pdMS_TO_TICKS(500));
    return result;
}

bool DEVICES::calibrateSeq()
{
    return true;
}

bool DEVICES::checkRequirementsMet()
{
    uint8_t statusMask = 0;
    if (xSemaphoreTake(m_statusMaskMutex, portMAX_DELAY))
    {
        statusMask = m_statusMask;
        xSemaphoreGive(m_statusMaskMutex);
    }

    bool isIndicationOk = (statusMask & INDICATION_BIT) == INDICATION_BIT;
    // bool isUsbpdOk = (statusMask & USBPD_BIT) == USBPD_BIT;
    bool isBldcOk = (statusMask & BLDC_BIT) == BLDC_BIT;
    bool isImuOrEncOk = ((statusMask & IMU_BIT) == IMU_BIT) || ((statusMask & ROT_ENC_BIT) == ROT_ENC_BIT);
    // ESP_LOGI("DEVICES", "Indication: %d, USBPD: %d, BLDC: %d, IMU/ENC: %d", isIndicationOk, isUsbpdOk, isBldcOk, isImuOrEncOk);

    return isIndicationOk && isBldcOk && isImuOrEncOk; // isUsbpdOk !!! for now we will remove this
}

void DEVICES::setStatus(uint8_t status)
{
    ESP_LOGE("DEVICES", "Setting status to (DELETE THIS): %d", status);

    if (xSemaphoreTake(m_statusMaskMutex, portMAX_DELAY))
    {
        m_statusMask = status;
        xSemaphoreGive(m_statusMaskMutex);
    }
}

uint8_t DEVICES::getStatus()
{
    uint8_t status = 0;

    if (xSemaphoreTake(m_statusMaskMutex, portMAX_DELAY))
    {
        status = m_statusMask;
        xSemaphoreGive(m_statusMaskMutex);
    }

    ESP_LOGI("DEVICES", "Status: %d", status);

    return status;
}

bool DEVICES::sleepMode()
{
    bool deviceSLeepSucc = true;

    if (!deviceSLeepSucc)
    {
        ESP_LOGE("STATE_MACHINE", "Failed to sleep devices!");
        // Handle error accordingly
    }

    return deviceSLeepSucc;
}

void DEVICES::wakeMode()
{
}