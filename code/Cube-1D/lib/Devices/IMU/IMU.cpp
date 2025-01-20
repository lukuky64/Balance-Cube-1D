#include "IMU.hpp"

#if DUMMY_IMU

#else

/*****************************************************************************/
/*!
    @brief Constructor for IMU class

    @param intPin The GPIO pin to be used for the interrupt

    @return True on successful configuration
*/
/*****************************************************************************/
IMU::IMU() : m_initialised(false)
{
    m_accelRange = LSM6DS_ACCEL_RANGE_2_G;
    m_gyroRange = LSM6DS_GYRO_RANGE_125_DPS;
    m_dataRate = LSM6DS_RATE_416_HZ; // also min requirement for high-performance mode

    m_accelMutex = xSemaphoreCreateMutex();
    m_gyroMutex = xSemaphoreCreateMutex();
    m_tempMutex = xSemaphoreCreateMutex();
}

/*****************************************************************************/
/*!
    @brief  Initialises the sensor with SPI and sets sensor data rate and range

    @param  SPI_CS The device's chip select / enable pin

    @param  SPI The SPI object to be used for SPI connections.

    @return True on successful initialisation
*/
/*****************************************************************************/

bool IMU::begin(uint8_t SPI_CS, SPICOM &SPI, gpio_num_t intPin)
{

    m_SPI = &SPI;

    SemaphoreGuard guard(m_SPI->mutex);
    if (guard.acquired())
    {
        if (m_imu.begin_SPI(SPI_CS, m_SPI->BUS))
        {
            m_imu.setAccelRange(m_accelRange);
            m_imu.setGyroRange(m_gyroRange);
            m_imu.setAccelDataRate(m_dataRate);
            m_imu.setGyroDataRate(m_dataRate);
            m_initialised = configureInturrupt(intPin);
        }
    }
    return m_initialised;
}

/*****************************************************************************/
/*!
    @brief  Update with the latest the sensor readings

    @return True on successful event capture
*/
/*****************************************************************************/
bool IMU::update()
{
    bool success = false;

    SemaphoreGuard guard(m_SPI->mutex);
    if (guard.acquired())
    {
        SemaphoreGuard guard1(m_accelMutex);
        SemaphoreGuard guard2(m_gyroMutex);
        SemaphoreGuard guard3(m_tempMutex);

        if (guard1.acquired() && guard2.acquired() && guard3.acquired())
        {
            success = m_imu.getEvent(&m_accel, &m_gyro, &m_temp); // updating all data
        }
    }

    return success;
}

/***************************************************************************************/
/*!
    @brief  Accesses the latest saved reading from accelerometer after calling update()

    @return Float value of acceleration in x-axis  (m/s^2)
*/
/***************************************************************************************/
float IMU::getAccelX()
{
    SemaphoreGuard guard(m_accelMutex);
    if (guard.acquired())
    {
        return m_accel.acceleration.x;
    }
    return 0.0f;
}

/***************************************************************************************/
/*!
    @brief  Accesses the latest saved reading from accelerometer after calling update()

    @return Float value of acceleration in y-axis (m/s^2)
*/
/***************************************************************************************/
float IMU::getAccelY()
{
    SemaphoreGuard guard(m_accelMutex);
    if (guard.acquired())
    {
        return m_accel.acceleration.y;
    }
    return 0.0f;
}

/***************************************************************************************/
/*!
    @brief  Accesses the latest saved reading from accelerometer after calling update()

    @return Float value of acceleration in z-axis (m/s^2)
*/
/***************************************************************************************/
float IMU::getAccelZ()
{
    SemaphoreGuard guard(m_accelMutex);
    if (guard.acquired())
    {
        return m_accel.acceleration.z;
    }
    return 0.0f;
}

/***************************************************************************************/
/*!
    @brief  Accesses the latest saved reading from accelerometer after calling update()

    @return Float value of gyro in x-axis (rad/s)
*/
/***************************************************************************************/
float IMU::getGyroX()
{
    SemaphoreGuard guard(m_gyroMutex);
    if (guard.acquired())
    {
        return m_gyro.gyro.x;
    }
    return 0.0f;
}

/***************************************************************************************/
/*!
    @brief  Accesses the latest saved reading from accelerometer after calling update()

    @return Float value of gyro in y-axis (rad/s)
*/
/***************************************************************************************/
float IMU::getGyroY()
{
    SemaphoreGuard guard(m_gyroMutex);
    if (guard.acquired())
    {
        return m_gyro.gyro.y;
    }
    return 0.0f;
}

/***************************************************************************************/
/*!
    @brief  Accesses the latest saved reading from accelerometer after calling update()

    @return Float value of gyro in z-axis (rad/s)
*/
/***************************************************************************************/
float IMU::getGyroZ()
{
    SemaphoreGuard guard(m_gyroMutex);
    if (guard.acquired())
    {
        return m_gyro.gyro.z;
    }
    return 0.0f;
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

/*****************************************************************************/
/*!
    @brief  Configures the IMU to generate an interrupt on the specified GPIO pin

    @param intPin The GPIO pin to be used for the interrupt

    @return True on successful configuration
*/
/*****************************************************************************/
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

/**************************************************************************/
/*!
    @brief  Gets the GPIO pin number used for the interrupt

    @return GPIO pin number
*/
/**************************************************************************/
gpio_num_t IMU::getIntPin()
{
    return m_intPin;
}

#endif

float IMU::getOmega()
{
#if OMEGA_SET_Y_AXIS
    return getGyroY();
#elif OMEGA_SET_X_AXIS
    return getGyroX();
#elif OMEGA_SET_Z_AXIS
    return getGyroZ();
#endif
}
