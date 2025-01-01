
#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "Arduino.h"
#include "esp_log.h"

#include "DEVICES.hpp"

enum STATES
{
    INITIALISATION = 0,
    CRITICAL_ERROR = 1,
    CALIBRATION = 2,
    LOW_POWER_IDLE = 3,
    CONTROL = 4,
};

enum PRIORITY
{
    PRIORITY_HIGH = 1,
    PRIORITY_MEDIUM = 2,
    PRIORITY_LOW = 3,
};

class STATE_MACHINE
{
public:
    STATE_MACHINE();
    ~STATE_MACHINE();

    void setup();
    void begin();
    void loop();

    void calibrationSeq();
    void controlSeq();
    void lowPowerIdleSeq();

    STATES getCurrentState();

    void criticalErrorSeq();

    // FreeRTOS Tasks
    static void indicationLoopTask(void *pvParameters);
    static void checkStatusTask(void *pvParameters);

private:
    STATES m_currState;

    DEVICES m_devices;
};

#endif // STATE_MACHINE_HPP