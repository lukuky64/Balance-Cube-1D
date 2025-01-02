// CYPD3177 Arduino library
// created 20.03.2019
// Copyright (c) 2019 Wolfgnag Friedrich <wolfgangfriedrich42@gmail.com>.  All right reserved.
// Released under Creative Commons Attribution-Share Alike 4.0 International License

// Pier 42 Watt-A-Live Shield/Wing

// Last change: 2019/Oct/22

// https://www.tindie.com/stores/pier42/
// https://hackaday.io/project/166326-watt-a-live-power-monitor-shield-wing
// https://github.com/wolfgangfriedrich/P42-Watt-A-Live

#include <Arduino.h>
#include "USB_PD.hpp"

USB_PD::USB_PD(void)
{
}

// read a word from the register
word USB_PD::readWord(byte reg_addr)
{

    byte MSB = 0;
    byte LSB = 0;

    Wire.beginTransmission(CYPD3177_ADDR);
    Wire.write(lowByte(reg_addr));
    Wire.write(highByte(reg_addr));
    Wire.endTransmission();
    Wire.beginTransmission(CYPD3177_ADDR);
    Wire.requestFrom(CYPD3177_ADDR, 2); // read 2 bytes from register
    LSB = Wire.read();
    MSB = Wire.read();
    Wire.endTransmission();
    return word(MSB, LSB);
}

// write a word into the register pointed
void USB_PD::writeWord(byte reg_addr, word data)
{
    Wire.beginTransmission(CYPD3177_ADDR);
    Wire.write(lowByte(reg_addr));
    Wire.write(highByte(reg_addr));
    Wire.write(highByte(data));
    Wire.write(lowByte(data));
    Wire.endTransmission();
}

bool USB_PD::checkStatus()
{
    return false;
}