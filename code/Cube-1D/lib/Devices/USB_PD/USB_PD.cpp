#include <Arduino.h>
#include "USB_PD.hpp"

USB_PD::USB_PD(void) : m_voltage(0.0)
{
}

// read a word from the register
bool USB_PD::readRegister(registerInfo register_, uint8_t *buffer)
{
    ESP_LOGI("USB_PD", "Reading from register: 0x%04X", register_.address);

    // Write the register address (16-bit)
    m_I2C->BUS->beginTransmission(ADDR);
    m_I2C->BUS->write(lowByte(register_.address));  // Send LSB of register address
    m_I2C->BUS->write(highByte(register_.address)); // Send MSB of register address

    if (m_I2C->BUS->endTransmission(false) != 0) // m_I2C->BUS->endTransmission(false) this should be needed
    {
        ESP_LOGE("USB_PD", "Failed to send register address.");
        return false;
    }

    // Request `size` bytes from the register
    size_t bytes_read = m_I2C->BUS->requestFrom(ADDR, register_.size);

    ESP_LOGI("USB_PD", "Bytes read: %u (Expected: %u)", bytes_read, register_.size);
    if (bytes_read == register_.size)
    {
        for (size_t i = 0; i < register_.size; i++)
        {
            buffer[i] = m_I2C->BUS->read();
            ESP_LOGI("USB_PD", "Buffer[%u]: 0x%02X", i, buffer[i]);
        }
        return true;
    }

    ESP_LOGE("USB_PD", "Failed to read expected number of bytes.");
    return false;
}

// write a word into the register pointed.
bool USB_PD::writeWord(registerInfo register_, word data)
{
    // The LSB of register address is transferred first followed by the MSB
    m_I2C->BUS->beginTransmission(ADDR);
    m_I2C->BUS->write(lowByte(register_.address));
    m_I2C->BUS->write(highByte(register_.address));
    m_I2C->BUS->write(lowByte(data));
    m_I2C->BUS->write(highByte(data));
    if (m_I2C->BUS->endTransmission() == 0) // 0 indicates success
    {
        return true;
    }
    return false;
}

bool USB_PD::checkStatus()
{
    return true;
}

bool USB_PD::begin(I2CCOM &I2C)
{
    m_I2C = &I2C;
    return updateVoltage();
}

bool USB_PD::updateVoltage()
{
    uint8_t buffer[1]; // BUS_VOLTAGE is 1 byte

    if (readRegister(Registers::BUS_VOLTAGE, buffer))
    {
        // ESP_LOGI("USB_PD", "Raw buffer[0]: 0x%02X", buffer[0]);
        m_voltage = (float)buffer[0] / 10.0; // Convert to volts (100 mV units)
        ESP_LOGI("USB_PD", "Voltage: %f", m_voltage);
        return true;
    }
    return false; // Failed to read
}

float USB_PD::getVoltage()
{
    return m_voltage;
}
