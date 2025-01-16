#pragma once

/*
CYPD3177 DATASHEET: https://www.infineon.com/dgdl/Infineon-EZ-PD_BCR_Datasheet_USB_Type-C_Port_Controller_for_Power_Sinks-DataSheet-v03_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0ee7ce9d70ad
Interface documentation: https://www.infineon.com/dgdl/Infineon-EZ-PD_BCR_Host_Processor_Interface_Specification-Software-v01_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f8c4313766b&da=t
*/

#include <Arduino.h>
#include <Comms/Comms.hpp>
#include "SemaphoreGuard.hpp"

class USB_PD
{

public:
    USB_PD(void);
    //	TI_INA209(byte address, float shunt);
    bool readRegister(word reg_addr, uint8_t *buffer, size_t length);
    bool writeWord(word reg_addr, word data);
    bool checkStatus();
    bool begin(I2CCOM &I2C);
    float getVoltage();

private:
    I2CCOM *m_I2C = nullptr;

    float m_voltage;

    static constexpr uint8_t ADDR = 0x08; // 7-bit I2C address (always this value)

    struct Registers
    {
        // static constexpr uint16_t DEVICE_MODE = 0x0000; // Device Mode
        // static constexpr uint16_t SILICON_ID = 0x0002;  // Silicon ID

        // static constexpr uint16_t RESET = 0x0008;       // To reset the device. This may cause system to lose power if we use this.
        static constexpr uint16_t BUS_VOLTAGE = 0x100D; // Reports live voltage on VBUS (in 100mV units) using internal ADC on the BCR
    };

    // struct Config
    // {
    //     static constexpr uint16_t RST = 0x8000;  // Reset bit
    //     static constexpr uint16_t BRNG = 0x2000; // Bus voltage range
    //     static constexpr uint16_t PGA = 0x1800;  // Shunt voltage mask
    //     static constexpr uint16_t BADC = 0x0780; // Bus ADC Resolution/Averaging
    //     static constexpr uint16_t SADC = 0x0078; // Shunt ADC Resolution/Averaging
    //     static constexpr uint16_t MODE = 0x0007; // Operating Mode
    // };

    bool updateVoltage();
};
