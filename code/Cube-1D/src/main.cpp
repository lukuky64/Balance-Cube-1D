#include <Arduino.h>
#include "esp_log.h"
#include "State_Machine.hpp"
#include "esp_heap_caps.h"

State_Machine stateMachine;

void setup()
{
    heap_caps_check_integrity_all(true);
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    delay(4000);

    ESP_LOGI("CHECK", "CPU FREQ [%d] MHz", ESP.getCpuFreqMHz());
    ESP_LOGI("CHECK", "NUM CORES [%d]", ESP.getChipCores());

    stateMachine.begin(); // Start the state machine
}

void loop()
{
    // Empty if we want all logic in FreeRTOS tasks
}
