
#ifndef DEVICES_HPP
#define DEVICES_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "Arduino.h"

#include "INDICATORS.hpp"
#include "USB_PD.hpp"
#include "BLDC.hpp"
#include "IMU.hpp"
#include "ROT_ENC.hpp"
#include "MAG_ENC.hpp"
#include "LOG.hpp"
#include "SERVO_CTR.hpp"

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
    bool setupUSBPD(uint8_t SCL, uint8_t SDA);
    bool setupBLDC();
    bool setupIMU(uint8_t CS, uint8_t MISO, uint8_t MOSI, uint8_t CLK);
    bool setupROT_ENC();
    bool setupMAG(uint8_t CS, uint8_t MISO, uint8_t MOSI, uint8_t CLK);
    bool setupSerialLog();
    bool setupSDLog(uint8_t CS, uint8_t MISO, uint8_t MOSI, uint8_t CLK);
    bool setupServo(uint8_t servoPin);

    void refreshStatusAll();
    bool indicateStatus();
    bool checkRequirementsMet();
    bool initialisationSeq(bool logSD, bool logSerial, bool SilentIndication, bool servoBraking, bool useIMU, bool useROT_ENC);

    void setStatus(uint8_t status);

private:
    uint8_t m_statusMask = 0;

    uint8_t m_prefMask = 0;

    // devices
    INDICATORS m_indicators;
    USB_PD m_usbPD;
    BLDC m_bldc;
    IMU m_imu;
    ROT_ENC m_rotEnc;
    MAG_ENC m_magEnc;
    LOG m_logger;
    SERVO_CTR m_servo;
};

#endif // DEVICES_HPP