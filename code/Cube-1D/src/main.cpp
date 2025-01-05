#include <Arduino.h>
#include "esp_log.h"
#include "State_Machine.hpp"

// Create an instance of the STATEMACHINE class
State_Machine stateMachine;

void setup()
{
    delay(4000);

    ESP_LOGI("CHECK", "CPU FREQ [%d] MHz", ESP.getCpuFreqMHz());
    ESP_LOGI("CHECK", "NUM CORES [%d]", ESP.getChipCores());

    // Call the begin function of the state machine
    stateMachine.begin();
}

void loop()
{
    // Remains empty if you want all logic in FreeRTOS tasks
}
