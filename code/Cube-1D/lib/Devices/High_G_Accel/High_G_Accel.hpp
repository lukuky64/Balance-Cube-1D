// #ifndef HIGH_G_ACCEL_HPP
// #define HIGH_G_ACCEL_HPP

// #include "Arduino.h"

// #if DUMMY_HIGH_G
// #include "esp_log.h"
// #include "driver/gpio.h"
// #include "COMMS.hpp"

// #else
// #include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_ADXL375.h>
// #include "COMMS.hpp"
// #include "SemaphoreGuard.hpp"
// #endif

// // Define constants for dummy HIGH_G_ACCEL
// #if DUMMY_HIGH_G
// #define DUMMY_ACCEL_X 0.0f
// #define DUMMY_ACCEL_Y 0.0f
// #define DUMMY_ACCEL_Z 1.0f // Assuming static gravity
// #define DUMMY_GYRO_X 0.0f
// #define DUMMY_GYRO_Y 0.1f
// #define DUMMY_GYRO_Z 0.0f
// #endif

// class HIGH_G_ACCEL
// {
// public:
// #if DUMMY_HIGH_G
//     // Dummy HIGH_G_ACCEL member functions with inline implementations
//     bool begin(uint8_t SPI_CS, SPICOM &SPI_BUS, gpio_num_t intPin)
//     {
//         ESP_LOGI("HIGH_G_ACCEL", "Dummy HIGH_G_ACCEL initialised.");
//         return true;
//     }

//     bool update() { return true; }

//     float getAccelX() { return DUMMY_ACCEL_X; }
//     float getAccelY() { return DUMMY_ACCEL_Y; }
//     float getAccelZ() { return DUMMY_ACCEL_Z; }

//     float getGyroX() { return DUMMY_GYRO_X; }
//     float getGyroY() { return DUMMY_GYRO_Y; }
//     float getGyroZ() { return DUMMY_GYRO_Z; }

//     uint32_t getTimestampMS() { return millis(); }

//     bool checkStatus() { return true; }

//     bool configureInturrupt(gpio_num_t intPin)
//     {
//         m_intPin = intPin;
//         ESP_LOGI("HIGH_G_ACCEL", "Dummy interrupt configured on pin %d", m_intPin);
//         return true;
//     }

//     gpio_num_t getIntPin() { return m_intPin; }

// private:
//     gpio_num_t m_intPin;

// #else
//     // Real HIGH_G_ACCEL member functions (declarations only)
//     HIGH_G_ACCEL();
//     bool begin(uint8_t SPI_CS, SPICOM &SPI_BUS);
//     bool update();
//     void print();
//     float getAccelX();
//     float getAccelY();
//     float getAccelZ();
//     float getGyroX();
//     float getGyroY();
//     float getGyroZ();
//     uint32_t getTimestampMS();
//     bool checkStatus();

//     bool configureInturrupt(gpio_num_t intPin);
//     gpio_num_t getIntPin();

// private:
//     Adafruit_ADXL375 *m_accel;

//     sensors_event_t event;

//     gpio_num_t m_intPin;

//     SPICOM *m_SPI_BUS = nullptr;

//     bool m_initialised;
// #endif
// };

// #endif // HIGH_G_ACCEL_HPP
