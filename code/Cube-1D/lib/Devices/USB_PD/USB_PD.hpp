#pragma once

#include <Arduino.h>
#include <Comms/Comms.hpp>
#include "SemaphoreGuard.hpp"
#include "params.hpp"

struct registerInfo
{
    uint16_t address; // register address (2-bytes)
    uint8_t size;     // bytes
};

struct Registers
{
    static constexpr registerInfo DEVICE_MODE = {0x0000, 1};   // Device Mode
    static constexpr registerInfo TYPE_C_STATUS = {0x100C, 4}; // Type-C Status
    static constexpr registerInfo SILICON_ID = {0x0002, 2};    // Silicon ID
    static constexpr registerInfo RESET = {0x0008, 1};         // To reset the device. This may cause system to lose power if we use this.
    static constexpr registerInfo BUS_VOLTAGE = {0x100D, 1};   // Reports live voltage on VBUS (in 100mV units) using internal ADC on the BCR
    static constexpr registerInfo EVENT_MASK = {0x1024, 4};    // Event Mask
};

/******************************************************************************************************************************************************************************************/
/*!
    @file
    @brief References and documentation for the CYPD3177 chip and its interface.

    @details
    - CYPD3177 DATASHEET: https://www.infineon.com/dgdl/Infineon-EZ-PD_BCR_Datasheet_USB_Type-C_Port_Controller_for_Power_Sinks-DataSheet-v03_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee7ce9d70ad
    - Interface documentation: https://www.infineon.com/dgdl/Infineon-EZ-PD_BCR_Host_Processor_Interface_Specification-Software-v01_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f8c4313766b&da=t
*/
/******************************************************************************************************************************************************************************************/
class USB_PD
{

public:
    USB_PD(void);
    //	TI_INA209(byte address, float shunt);
    bool readRegister(registerInfo register_, uint8_t *buffer);
    bool writeWord(registerInfo register_, word data);

#if DUMMY_USBPD
    bool checkStatus() { return true; }
    bool begin(I2CCOM &I2C) { return true; }
    float getVoltage() { return 12.0f; }
#else
    bool checkStatus();
    bool begin(I2CCOM &I2C);
    float getVoltage();

#endif

private:
    I2CCOM *m_I2C = nullptr;

    float m_voltage;

    static constexpr uint8_t ADDR = 0x08; // 7-bit I2C address (always this value)

    // struct Config
    // {
    //     static constexpr uint16_t RST = 0x8000;  // Reset bit
    //     static constexpr uint16_t BRNG = 0x2000; // Bus voltage range
    //     static constexpr uint16_t PGA = 0x1800;  // Shunt voltage mask
    //     static constexpr uint16_t BADC = 0x0780; // Bus ADC Resolution/Averaging
    //     static constexpr uint16_t SADC = 0x0078; // Shunt ADC Resolution/Averaging
    //     static constexpr uint16_t MODE = 0x0007; // Operating Mode
    // };

#if DUMMY_USBPD
    bool updateVoltage() { return true; }
#else
    bool updateVoltage();
#endif
};
