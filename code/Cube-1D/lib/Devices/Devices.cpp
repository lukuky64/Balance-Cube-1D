#include "Devices.hpp"

Devices::Devices()
{
    m_SPIComSensors.mutex = xSemaphoreCreateMutex();
    m_SPIComSD.mutex = xSemaphoreCreateMutex();

    m_statusMaskMutex = xSemaphoreCreateMutex();
    m_prefMaskMutex = xSemaphoreCreateMutex();
}

// bool Devices::setupIndication(bool silentIndication)
// {
//     return true;
// }

Devices::~Devices()
{
    // Disable or de-initialize all Devices if necessary

    // Delete mutexes
    if (m_SPIComSensors.mutex != NULL)
    {
        vSemaphoreDelete(m_SPIComSensors.mutex);
        m_SPIComSensors.mutex = NULL;
    }

    if (m_SPIComSD.mutex != NULL)
    {
        vSemaphoreDelete(m_SPIComSD.mutex);
        m_SPIComSD.mutex = NULL;
    }

    if (m_statusMaskMutex != NULL)
    {
        vSemaphoreDelete(m_statusMaskMutex);
        m_statusMaskMutex = NULL;
    }

    if (m_prefMaskMutex != NULL)
    {
        vSemaphoreDelete(m_prefMaskMutex);
        m_prefMaskMutex = NULL;
    }
}

bool Devices::setupUSBPD(gpio_num_t SCL, gpio_num_t SDA)
{
    if (setupI2C(SCL, SDA, m_I2CComPeripherals))
    {
        return m_usbPD.begin(m_I2CComPeripherals);
    }
    return false;
}

bool Devices::setupBLDC(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, int nFLT, int nSLEEP, Mag_Enc *mag_enc, float voltage, float Kv)
{
    // mag must be setup first. Make sure USBPD gets the voltage before this is used
    return m_bldc.begin(phA, phB, phC, enable, senseA, senseB, MAG_CS, nFLT, nSLEEP, mag_enc, voltage, Kv);
}

bool Devices::setupSPI(gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK, SPICOM &SPI)
{
    SemaphoreGuard guard(SPI.mutex);
    if (guard.acquired())
    {
        // check if SPI bus is already initialised
        if (!SPI.begun)
        {
            SPI.BUS->begin(CLK, MISO, MOSI);
            SPI.BUS->setFrequency(SPI.frequency); // 40 MHz
            SPI.begun = true;
        }
        return SPI.begun;
    }
    else
    {
        return false;
    }
}

bool Devices::setupI2C(gpio_num_t SCL, gpio_num_t SDA, I2CCOM &I2C)
{
    // skipping making mutex because its already doing that inside i2c
    // check if I2C bus is already initialised
    if (!I2C.begun)
    {
        bool i2c_status = I2C.BUS->begin(SDA, SCL, I2C.frequency);
        I2C.begun = i2c_status;
    }
    return I2C.begun;
}

bool Devices::setupIMU(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK, gpio_num_t intPin)
{
    if (!setupSPI(MISO, MOSI, CLK, m_SPIComSensors))
    {
        return false;
    }

    esp_err_t interrupt_setup = esp_sleep_enable_ext0_wakeup(intPin, 1); // 1 = High level wake-up using the IMU's interrupt pin

    if (interrupt_setup != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set up IMU interrupt pin %d", intPin);
        return false;
    }
    else
    {
        ESP_LOGI(TAG, "IMU interrupt pin %d set up successfully", intPin);
    }

    return m_imu.begin(CS, m_SPIComSensors, intPin);
}

bool Devices::setupROT_ENC()
{
    return true;
}

bool Devices::setupMAG(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK)
{
    if (!setupSPI(MISO, MOSI, CLK, m_SPIComSensors))
    {
        return false;
    }

    return m_magEnc.init(CS, m_SPIComSensors, AS5047_mag);
}

bool Devices::setupSerialLog()
{
    if (m_logger.m_serialTalker.begin())
    {
        m_logger.selectLogSerial();
        return true;
    }

    return false;
}

bool Devices::setupSDLog(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK)
{
    if (setupSPI(MISO, MOSI, CLK, m_SPIComSD))
    {
        if (m_logger.m_sdTalker.begin(CS, m_SPIComSD))
        {
            m_logger.selectLogSD();

            return true;
        }
    }

    return false;
}

bool Devices::setupServo(gpio_num_t servoPin)
{
    return true; // m_servo.begin(servoPin);
}

bool Devices::indicateStatus()
{
    bool requirementsMet = checkRequirementsMet();

    if (!requirementsMet)
    {
        m_indicators.showCriticalError();
        return requirementsMet;
    }

    uint8_t statusMask = getStatus();
    uint8_t prefMask = getPref();

    if ((statusMask & prefMask) != prefMask)
    {
        ESP_LOGI(TAG, "Not all prefs met!");
        m_indicators.showWarning();
        return requirementsMet;
    }

    m_indicators.showAllGood();
    return requirementsMet;
}

void Devices::refreshStatusAll() // !!!
{
    ESP_LOGI(TAG, "Checking all device statuses...");

    uint8_t statusMask = 0;
    statusMask |= (m_indicators.checkStatus() ? INDICATION_BIT : 0);
    statusMask |= (m_usbPD.checkStatus() ? USBPD_BIT : 0);
    statusMask |= (m_bldc.checkStatus() ? BLDC_BIT : 0);      // need to implement
    statusMask |= (m_imu.checkStatus() ? IMU_BIT : 0);        // need to implement
    statusMask |= (m_rotEnc.checkStatus() ? ROT_ENC_BIT : 0); // need to implement
    statusMask |= (m_magEnc.checkStatus() ? MAG_BIT : 0);     // need to implement
    statusMask |= (m_logger.m_serialTalker.checkStatus() ? SERIAL_BIT : 0);
    statusMask |= (m_logger.m_sdTalker.checkStatus() ? SD_BIT : 0);
    // statusMask |= (m_servo.checkStatus() ? SERVO_BIT : 0); // need to implement

    setStatus(statusMask);
}

bool Devices::init(bool logSD, bool logSerial, bool SilentIndication, bool servoBraking, bool useIMU, bool useROT_ENC)
{
    vTaskDelay(pdMS_TO_TICKS(100));

    bool result = false;

    uint8_t statusMask = 0;
    uint8_t prefMask = 0;

    // set up indication. If silent indication is enabled, only set up RGB LED
    {
        m_indicators.setupRGBLed(LED_NEO);

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

    // Magnetic sensor and valid voltage reading is required for BLDC
    if ((statusMask & MAG_BIT) == MAG_BIT)
    {
        if ((statusMask & USBPD_BIT) == USBPD_BIT)
        {
            // set up BLDC
            if (setupBLDC(BLDC_INA, BLDC_INB, BLDC_INC, BLDC_EN, BLDC_SENSE_A, BLDC_SENSE_B, SPI_CS_MAG, BLDC_nFLT, BLDC_nSLP, &m_magEnc, m_usbPD.getVoltage(), Params::MOTOR_KV))
            {
                statusMask |= BLDC_BIT;
                prefMask |= BLDC_BIT;
            }
        }
    }

    setStatus(statusMask);
    setPref(prefMask);

    uint8_t prefs_check = getPref();

    // explicitly check each required bit
    if (checkRequirementsMet())
    {
        ESP_LOGI("Initialisation", "Minimum device successes satisfied!");
        // m_indicators.showSuccess();
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

bool Devices::calibrateSeq()
{
    return true;
}

bool Devices::checkRequirementsMet()
{
    uint8_t statusMask = getStatus();

    bool isIndicationOk = (statusMask & INDICATION_BIT) == INDICATION_BIT;
    bool isUsbpdOk = (statusMask & USBPD_BIT) == USBPD_BIT;
    bool isBldcOk = (statusMask & BLDC_BIT) == BLDC_BIT;
    bool isImuOrEncOk = ((statusMask & IMU_BIT) == IMU_BIT) || ((statusMask & ROT_ENC_BIT) == ROT_ENC_BIT);
    // ESP_LOGI(TAG, "Indication: %d, USBPD: %d, BLDC: %d, IMU/ENC: %d", isIndicationOk, isUsbpdOk, isBldcOk, isImuOrEncOk);

    return isIndicationOk && isBldcOk && isImuOrEncOk && isUsbpdOk;
}

void Devices::setStatus(uint8_t status)
{
    // ESP_LOGI(TAG, "Setting Status: %d", status);
    {
        SemaphoreGuard guard(m_statusMaskMutex);
        if (guard.acquired())
        {
            m_statusMask = status;
        }
    }
}

void Devices::setPref(uint8_t pref)
{
    {
        SemaphoreGuard guard(m_prefMaskMutex);
        if (guard.acquired())
        {
            m_prefMask = pref;
        }
    }
}

uint8_t Devices::getStatus()
{
    uint8_t status = 0;
    {
        SemaphoreGuard guard(m_statusMaskMutex);
        if (guard.acquired())
        {
            status = m_statusMask;
        }
    }
    // ESP_LOGI(TAG, "Status: %u", status);
    return status;
}

uint8_t Devices::getPref()
{
    uint8_t pref = 0;
    {
        SemaphoreGuard guard(m_prefMaskMutex);
        if (guard.acquired())
        {
            pref = m_prefMask;
        }
    }
    // ESP_LOGI(TAG, "Pref: %d", pref);
    return pref;
}

bool Devices::sleepMode()
{
    bool deviceSLeepSucc = true;

    m_bldc.sleepMode(true);

    m_indicators.showAllOff();

    if (!deviceSLeepSucc)
    {
        ESP_LOGE("STATE_MACHINE", "Failed to sleep Devices!");
        // Handle error accordingly
    }
    else
    {
        ESP_LOGI("STATE_MACHINE", "Sleeping Devices!");
    }

    return deviceSLeepSucc;
}

void Devices::wakeMode()
{
    // quick beep
    m_bldc.sleepMode(false);

    m_indicators.showSuccess();
}

bool Devices::canSleep()
{
    uint8_t deviceStatus = getStatus();

    if ((deviceStatus & USBPD_BIT) == USBPD_BIT)
    {
        // check if IMU is enabled
        if ((deviceStatus & IMU_BIT) == IMU_BIT)
        {
            ESP_LOGI(TAG, "Can sleep!");
            return true;
        }
        else
        {
            ESP_LOGI(TAG, "IMU not enabled, cannot enter light sleep!");
            return false;
        }
    }
    else
    {
        ESP_LOGI(TAG, "USB connected, cannot enter light sleep!");
        return false;
    }
}