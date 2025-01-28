#include <Arduino.h>
#include "esp_log.h"
#include "State_Machine.hpp"

/**
 * @file BasicReadAngleAndDebugInfo.ino
 * @author Jonas Merkle [JJM] (jonas@jjm.one)
 * @brief This is a basic example program to read the angle position and debug information from a AS5047 rotary encoder.
 *        The angle postion and debug information gets updated and printed to the serial consol once a second.
 * @version 2.2.2
 * @date 2024-10-19
 *
 * @copyright Copyright (c) 2024 Jonas Merkle. This project is released under the GPL-3.0 License License.
 *
 * More Information can be found here:
 * https://github.com/jonas-merkle/AS5047P
 */

// include the library for the AS5047P sensor.
#include <AS5047P.h>

// define the spi bus speed
#define AS5047P_CUSTOM_SPI_BUS_SPEED 100000

// initialize a new AS5047P sensor object.
AS5047P as5047p(SPI_CS_MAG, AS5047P_CUSTOM_SPI_BUS_SPEED);

// arduino setup routine
void setup()
{
    // initialize the AS5047P sensor and hold if sensor can't be initialized.
    while (!as5047p.initSPI())
    {
        ESP_LOGE("as5047p", "Sensor initialization failed. Please check your wiring.");
        delay(5000);
    }
}

// arduino loop routine
void loop()
{
    ESP_LOGI("as5047p", "Angle: %f", as5047p.readAngleDegree());
    // ESP_LOGI("as5047p", "%s", as5047p.readStatusAsArduinoString().c_str()); // deactivate the led.
    delay(50);                                                             // wait for 500 milli seconds.
}