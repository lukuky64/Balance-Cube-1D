#include <Arduino.h>
#include "esp_log.h"
#include "STATE_MACHINE.hpp"

// Create an instance of the STATEMACHINE class
STATE_MACHINE stateMachine;

void setup()
{
    delay(2000);
    // Call the begin function of the state machine
    stateMachine.begin();
}

void loop()
{
    // Remains empty if you want all logic in FreeRTOS tasks
}
