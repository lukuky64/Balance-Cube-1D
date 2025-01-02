
#include "IMU.hpp"

IMU::IMU() : initialised(false)
{
    m_accelRange = LSM6DS_ACCEL_RANGE_2_G;
    m_gyroRange = LSM6DS_GYRO_RANGE_250_DPS;
    m_dataRate = LSM6DS_RATE_104_HZ;
}

/*****************************************************************************/
/*!
    @brief  Initialises the sensor with SPI and sets sensor data rate and range

    @param  SPI_CS The device's chip select / enable pin

    @param  pSPI_BUS The SPI object to be used for SPI connections.

    @return True on successful initialisation
*/
/*****************************************************************************/

bool IMU::begin(uint8_t SPI_CS, SPICOM &SPI_BUS, gpio_num_t intPin)
{

    m_SPI_BUS = &SPI_BUS;

    SemaphoreGuard guard(m_SPI_BUS->mutex);
    if (guard.acquired())
    {
        if (m_imu.begin_SPI(SPI_CS, m_SPI_BUS->BUS))
        {
            m_imu.setAccelRange(m_accelRange);
            m_imu.setGyroRange(m_gyroRange);
            m_imu.setAccelDataRate(m_dataRate);
            m_imu.setGyroDataRate(m_dataRate);
            initialised = configureInturrupt(intPin);
        }
    }
    return initialised;
}

bool IMU::update()
{
    bool success = false;

    SemaphoreGuard guard(m_SPI_BUS->mutex);
    if (guard.acquired())
    {
        success = m_imu.getEvent(&m_accel, &m_gyro, &m_temp);
    }

    return success;
}

float IMU::getAccelX()
{
    return m_accel.acceleration.x;
}

float IMU::getAccelY()
{
    return m_accel.acceleration.y;
}

float IMU::getAccelZ()
{
    return m_accel.acceleration.z;
}

float IMU::getGyroX()
{
    return m_gyro.gyro.x;
}

float IMU::getGyroY()
{
    return m_gyro.gyro.y;
}

float IMU::getGyroZ()
{
    return m_gyro.gyro.z;
}

/**************************************************************************/
/*!
    @brief  Gets the timestamp of the most recent sensor event

    @return timestamp in milliseconds
*/
/**************************************************************************/
uint32_t IMU::getTimestampMS()
{
    return m_accel.timestamp; // all sensor events have the same timestamp
}

bool IMU::checkStatus()
{
    return true;
}

bool IMU::configureInturrupt(gpio_num_t intPin)
{
    m_imu.configIntOutputs(false, false);               // active high and push-pull configuration
    m_imu.configInt1(false, false, false, false, true); // enabling wake up interrupt
    m_imu.enableWakeup(true, 50, 20);                   // duration, threshold

    m_intPin = intPin;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << m_intPin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // since we have push-pull configuration, don't need pull-down
        .intr_type = GPIO_INTR_DISABLE         // Disable interrupts; wake-up is handled by ESP-IDF
    };

    esp_err_t ret = gpio_config(&io_conf);

    if (ret != ESP_OK)
    {
        ESP_LOGE("IMU", "GPIO configuration failed with error: %d", ret);
        return false;
    }
    else
    {
        ESP_LOGI("IMU", "Configured GPIO %d for IMU interrupt", m_intPin);
        return true;
    }
}

gpio_num_t IMU::getIntPin()
{
    return m_intPin;
}