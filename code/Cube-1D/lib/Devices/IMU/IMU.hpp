#pragma once

#include "Arduino.h"
#include "Comms/Comms.hpp"
#include <Adafruit_LSM6DSOX.h>
#include "SemaphoreGuard.hpp"
#include "esp_log.h"
#include "driver/gpio.h"

// Define constants for dummy IMU
#if DUMMY_IMU
#define DUMMY_ACCEL_X 0.0f
#define DUMMY_ACCEL_Y 0.0f
#define DUMMY_ACCEL_Z 1.0f // Assuming static gravity
#define DUMMY_GYRO_X 0.0f
#define DUMMY_GYRO_Y 0.08f
#define DUMMY_GYRO_Z 0.0f
#endif

class IMU
{
public:
#if DUMMY_IMU
    // Dummy IMU member functions with inline implementations
    bool begin(uint8_t SPI_CS, SPICOM &SPI_BUS, gpio_num_t intPin)
    {
        ESP_LOGI("IMU", "Dummy IMU initialised.");
        return true;
    }

    bool update() { return true; }

    float getAccelX() { return DUMMY_ACCEL_X; }
    float getAccelY() { return DUMMY_ACCEL_Y; }
    float getAccelZ() { return DUMMY_ACCEL_Z; }

    float getGyroX() { return DUMMY_GYRO_X; }
    float getGyroY() { return DUMMY_GYRO_Y; }
    float getGyroZ() { return DUMMY_GYRO_Z; }

    uint32_t getTimestampMS() { return millis(); }

    bool checkStatus() { return true; }

    bool configureInturrupt(gpio_num_t intPin)
    {
        m_intPin = intPin;
        ESP_LOGI("IMU", "Dummy interrupt configured on pin %d", m_intPin);
        return true;
    }

    gpio_num_t getIntPin() { return m_intPin; }

private:
    gpio_num_t m_intPin;

#else
    // Real IMU member functions (declarations only)
    IMU();
    bool begin(uint8_t SPI_CS, SPICOM &SPI_BUS, gpio_num_t intPin);
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

    SemaphoreHandle_t m_accelMutex = NULL;
    SemaphoreHandle_t m_gyroMutex = NULL;
    SemaphoreHandle_t m_tempMutex = NULL;

    sensors_event_t m_accel;
    sensors_event_t m_gyro;
    sensors_event_t m_temp;

    gpio_num_t m_intPin;

    SPICOM *m_SPI_BUS = nullptr;

    bool m_initialised;
#endif
};
