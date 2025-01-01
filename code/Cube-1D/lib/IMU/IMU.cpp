
#include "IMU.hpp"

IMU::IMU()
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

bool IMU::begin(uint8_t SPI_CS, SPIClass *pSPI_BUS)
{
    if (m_imu.begin_SPI(SPI_CS, pSPI_BUS))
    {
        m_imu.setAccelRange(m_accelRange);
        m_imu.setGyroRange(m_gyroRange);
        m_imu.setAccelDataRate(m_dataRate);
        m_imu.setGyroDataRate(m_dataRate);
        return true;
    }
    else
    {
        return false;
    }
}

void IMU::update()
{
    m_imu.getEvent(&m_accel, &m_gyro, &m_temp);

    // m_accel.acceleration.x;
    // m_accel.acceleration.y;
    // m_accel.acceleration.z;

    // m_gyro.gyro.x;
    // m_gyro.gyro.y;
    // m_gyro.gyro.z;
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