
#ifndef BLDC_CTR_HPP
#define BLDC_CTR_HPP

#include "Arduino.h"
#include "esp_log.h"
#include <SimpleFOC.h>

class BLDC_CTR
{
public:
    BLDC_CTR();
    ~BLDC_CTR();
    bool checkStatus();
    void enableMotor(bool enable);
    void begin();

private:
    // 24N22P, how many pole pairs are there?

    // setting pp and R since i can find them in the datasheet
    BLDCMotor motor = BLDCMotor(11, 11.1);
    BLDCDriver3PWM driver = BLDCDriver3PWM(6, 10, 5, 8);
    // Encoder encoder = Encoder(2, 3, 500);
    MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, 10);

    // ACS712-05B has the resolution of 0.185mV per Amp
    InlineCurrentSense current_sense = InlineCurrentSense(185.0f, A0, A2);
};

#endif // BLDC_CTR_HPP