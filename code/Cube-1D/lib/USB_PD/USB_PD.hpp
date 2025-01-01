#ifndef USB_PD_HPP
#define USB_PD_HPP

// this is for the CYPD3177

#include <Arduino.h>
#include <Wire.h>

#define CYPD3177_ADDR 0x08 // 7 bit I2C address

// Register Name		Function			power on reset bin	hex		type
#define DEVICE_MODE 0x0000 // Device Mode   00000000 10010020	0092	R
#define SILICON_ID 0x0002  // SILICON ID	00010001 10110000	11B0	R

// Config reg 0x00
#define RST 0x8000  // Reset bit
#define BRNG 0x2000 // Bus voltage range
#define PGA 0x1800  // Shunt voltage mask
#define BADC 0x0780 // Bus ADC Resolution/Averaging
#define SADC 0x0078 // Shunt ADC Resolution/Averaging
#define MODE 0x0007 // Operating Mode

class USB_PD
{

public:
    USB_PD(void);
    //	TI_INA209(byte address, float shunt);
    word readWord(byte reg_addr);
    void writeWord(byte reg_addr, word data);
    bool checkStatus();
};

#endif // USB_PD_HPP