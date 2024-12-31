#ifndef IMU_HPP
#define IMU_HPP

#include "Arduino.h"
#include <Adafruit_LSM6DSOX.h>

class IMU
{
public:
    IMU();
    bool begin(uint8_t SPI_CS, SPIClass *spi);
    void update();
    void print();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
    float getGyroX();
    float getGyroY();
    float getGyroZ();
    uint32_t getTimestampMS();

private:
    Adafruit_LSM6DSOX m_imu;

    accel_range m_accelRange;
    gyro_range m_gyroRange;
    data_rate m_dataRate;

    sensors_event_t m_accel;
    sensors_event_t m_gyro;
    sensors_event_t m_temp;
};

#endif // IMU_HPP