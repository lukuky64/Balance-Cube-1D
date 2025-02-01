#include <Arduino.h>
#include "esp_log.h"
#include "State_Machine.hpp"
#include "params.hpp"

State_Machine stateMachine;

void setup()
{
    delay(1000);

    Params::loadPreferences(); // Load preferences from NVS
    stateMachine.begin();      // Start the state machine
}

void loop()
{
    vTaskDelay(portMAX_DELAY);
}
