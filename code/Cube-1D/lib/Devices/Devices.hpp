#pragma once

#include "freertos/FreeRTOS.h"
#include "SemaphoreGuard.hpp"

#include "esp_log.h"

#include "Arduino.h"

#include "Pin_Defs.hpp"

#include "Indicators/Indicators.hpp"
#include "USB_PD/USB_PD.hpp"
#include "BLDC/BLDC_CTR.hpp"
#include "IMU/IMU.hpp"
#include "Rot_Enc/Rot_Enc.hpp"
#include "Mag_Enc/Mag_Enc.hpp"
#include "Log/LOG.hpp"
#include "Servo/Servo_CTR.hpp"
#include "Comms/Comms.hpp"
#include "Device_messages.hpp"

enum DeviceBit
{
    INDICATION_BIT = 1 << 7, // 0b10000000
    USBPD_BIT = 1 << 6,      // 0b01000000
    BLDC_BIT = 1 << 5,       // 0b00100000
    IMU_BIT = 1 << 4,        // 0b00010000
    ROT_ENC_BIT = 1 << 3,    // 0b00001000
    MAG_BIT = 1 << 2,        // 0b00000100
    SERIAL_BIT = 1 << 1,     // 0b00000010
    SD_BIT = 1 << 0,         // 0b00000001
    SERVO_BIT = 0            // Not used for version 1.0 of Cube-1D
};

class Devices
{
public:
    Devices();
    ~Devices();
    // bool setupIndication(bool silentIndication);
    bool setupUSBPD(gpio_num_t SCL, gpio_num_t SDA);
    bool setupBLDC(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK);
    bool setupIMU(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK, gpio_num_t intPin);
    bool setupROT_ENC();
    bool setupMAG(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK);
    bool setupSerialLog();
    bool setupSDLog(gpio_num_t CS, gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK);
    bool setupServo(gpio_num_t servoPin);

    void refreshStatusAll();
    bool indicateStatus();
    bool checkRequirementsMet();
    bool init(bool logSD, bool logSerial, bool SilentIndication, bool servoBraking, bool useIMU, bool useROT_ENC);

    uint8_t getStatus();
    uint8_t getPref();

    void setPref(uint8_t status);

    bool calibrateSeq();

    bool sleepMode();
    void wakeMode();

    bool canSleep();

    bool setupSPI(gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK, SPICOM &SPI);
    bool setupI2C(gpio_num_t SCL, gpio_num_t SDA, I2CCOM &I2C);

    // devices. Making public for now
    Indicators m_indicators;
    USB_PD m_usbPD;
    BLDC_CTR m_bldc;
    IMU m_imu;
    Rot_Enc m_rotEnc;
    Mag_Enc m_magEnc;
    Log m_logger;
    Servo_CTR m_servo;

private:
    void setStatus(uint8_t status);

    SemaphoreHandle_t m_statusMaskMutex = NULL;
    uint8_t m_statusMask = 0;

    SemaphoreHandle_t m_prefMaskMutex = NULL;
    uint8_t m_prefMask = 0;

    static constexpr const char *TAG = "Devices";
};