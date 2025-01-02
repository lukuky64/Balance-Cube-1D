
#ifndef DEVICES_HPP
#define DEVICES_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "Arduino.h"

#include "PIN_DEFS.hpp"

#include "INDICATORS.hpp"
#include "USB_PD.hpp"
#include "BLDC.hpp"
#include "IMU.hpp"
#include "ROT_ENC.hpp"
#include "MAG_ENC.hpp"
#include "LOG.hpp"
#include "SERVO_CTR.hpp"
#include "COMMS.hpp"
#include "SemaphoreGuard.hpp"

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

class DEVICES
{
public:
    DEVICES();
    ~DEVICES();
    // bool setupIndication(bool silentIndication);
    bool setupUSBPD(gpio_num_t SCL, gpio_num_t SDA);
    bool setupBLDC();
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

    void setStatus(uint8_t status);
    void setPref(uint8_t status);

    bool calibrateSeq();

    bool sleepMode();
    void wakeMode();

    bool setupSPI(gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK, SPICOM &SPIBus);

    // devices. Making public for now
    INDICATORS m_indicators;
    USB_PD m_usbPD;
    BLDC m_bldc;
    IMU m_imu;
    ROT_ENC m_rotEnc;
    MAG_ENC m_magEnc;
    LOG m_logger;
    SERVO_CTR m_servo;

private:
    SemaphoreHandle_t m_statusMaskMutex = nullptr;
    uint8_t m_statusMask = 0;

    SemaphoreHandle_t m_prefMaskMutex = nullptr;
    uint8_t m_prefMask = 0;

    SPICOM m_SPIComSensors = {&SPI, false, nullptr};
    SPICOM m_SPIComSD = {&SPI, false, nullptr}; // sd will use a different bus
};

#endif // DEVICES_HPP