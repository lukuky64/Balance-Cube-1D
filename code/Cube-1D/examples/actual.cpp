#include <Arduino.h>
#include "esp_log.h"
#include "State_Machine.hpp"

State_Machine stateMachine;

void setup()
{
    delay(3000);
    stateMachine.begin(); // Start the state machine
}

void loop()
{
    // Empty if we want all logic in FreeRTOS tasks
}
