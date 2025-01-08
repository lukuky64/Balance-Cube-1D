
#ifndef DEVICES_HPP
#define DEVICES_HPP

#include "freertos/FreeRTOS.h"
#include "SemaphoreGuard.hpp"

#include "esp_log.h"

#include "Arduino.h"

#include "Pin_Defs.hpp"

#include "Indicators.hpp"
#include "USB_PD.hpp"
#include "BLDC_CTR.hpp"
#include "IMU.hpp"
#include "Rot_Enc.hpp"
#include "Mag_Enc.hpp"
#include "LOG.hpp"
#include "Servo_CTR.hpp"
#include "Comms.hpp"

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

    bool setupSPI(gpio_num_t MISO, gpio_num_t MOSI, gpio_num_t CLK, SPICOM &SPIBus);

    // devices. Making public for now
    Indicators &m_indicators = *new Indicators();
    USB_PD &m_usbPD = *new USB_PD();
    BLDC_CTR &m_bldc = *new BLDC_CTR();
    IMU &m_imu = *new IMU();
    Rot_Enc &m_rotEnc = *new Rot_Enc();
    Mag_Enc &m_magEnc = *new Mag_Enc();
    Log &m_logger = *new Log();
    Servo_CTR &m_servo = *new Servo_CTR();

private:
    void setStatus(uint8_t status);

    SemaphoreHandle_t m_statusMaskMutex = NULL;
    uint8_t m_statusMask = 0;

    SemaphoreHandle_t m_prefMaskMutex = NULL;
    uint8_t m_prefMask = 0;
};

#endif // DEVICES_HPP