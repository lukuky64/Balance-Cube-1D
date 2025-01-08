#include <Arduino.h>
#include "esp_log.h"
#include "State_Machine.hpp"

State_Machine stateMachine;

void setup()
{
    delay(4000);

    ESP_LOGI("CHECK", "CPU FREQ [%d] MHz", ESP.getCpuFreqMHz());
    ESP_LOGI("CHECK", "NUM CORES [%d]", ESP.getChipCores());

    stateMachine.begin(); // Start the state machine
}

void loop()
{
    // Empty if we want all logic in FreeRTOS tasks
}
