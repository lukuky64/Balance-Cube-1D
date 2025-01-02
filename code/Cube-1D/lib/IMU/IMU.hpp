#ifndef IMU_HPP
#define IMU_HPP

#include "Arduino.h"
#include <Adafruit_LSM6DSOX.h>
#include "esp_log.h"
#include "driver/gpio.h"

class IMU
{
public:
    IMU();
    bool begin(uint8_t SPI_CS, SPIClass *pSPI_BUS, gpio_num_t intPin);
    bool update();
    void print();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
    float getGyroX();
    float getGyroY();
    float getGyroZ();
    uint32_t getTimestampMS();
    bool checkStatus();

    bool configureInturrupt(gpio_num_t intPin);
    gpio_num_t getIntPin();

private:
    Adafruit_LSM6DSOX m_imu;

    accel_range m_accelRange;
    gyro_range m_gyroRange;
    data_rate m_dataRate;

    sensors_event_t m_accel;
    sensors_event_t m_gyro;
    sensors_event_t m_temp;

    gpio_num_t m_intPin;
};

#endif // IMU_HPP