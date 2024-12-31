
#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "Arduino.h"
#include "esp_log.h"

#include "DEVICES.hpp"

enum States
{
    INITIALISATION = 0,
    CRITICAL_ERROR = 1,
    CALIBRATION = 2,
    LOW_POWER_IDLE = 3,
    CONTROL = 4,
};

enum DeviceBit
{
    INDICATION_BIT = 1 << 7, // 0b10000000
    VOLTAGE_BIT = 1 << 6,    // 0b01000000
    BLDC_BIT = 1 << 5,       // 0b00100000
    IMU_BIT = 1 << 4,        // 0b00010000
    ROT_ENC_BIT = 1 << 3,    // 0b00001000
    MAG_BIT = 1 << 2,        // 0b00000100
    SERIAL_BIT = 1 << 1,     // 0b00000010
    SD_BIT = 1 << 0,         // 0b00000001
    SERVO_BIT = 0            // Not used for version 1.0 of Cube-1D
};

struct successes
{
    bool indicationSucc = false;
    bool voltageSucc = false;
    bool BLDCSucc = false;
    bool IMUSucc = false;
    bool ROT_ENCSucc = false;
    bool MAGSucc = false;
    bool serialSucc = false;
    bool SDSucc = false;
    bool ServoSucc = false;
};

struct preferences
{
    bool logSD = false;
    bool logSerial = false;
    bool SilentIndication = false;
    bool servoBraking = false;
    bool useIMU = false;
    bool useROT_ENC = false;
};

class STATEMACHINE
{
public:
    STATEMACHINE();
    ~STATEMACHINE();

    void setup(bool logSD, bool logSerial, bool SilentIndication, bool servoBraking, bool useIMU, bool useROT_ENC);
    void begin();
    void loop();

    void initialisationTask();
    void calibrationTask();
    void controlTask();
    void lowPowerIdleTask();

    States getCurrentState();

    void criticalErrorTask();

private:
    DEVICES m_devices;

    States m_currState;
    successes m_currSucc;
    preferences m_currPref;

    uint8_t m_requiredMask = INDICATION_BIT && VOLTAGE_BIT && BLDC_BIT && (IMU_BIT || ROT_ENC_BIT);
};

#endif // STATEMACHINE_HPP