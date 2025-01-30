#include <Arduino.h>
#include "esp_log.h"
#include "State_Machine.hpp"
#include "params.hpp"
#include "CLI.hpp"

State_Machine stateMachine;
// CLI cli = CLI(115200);

void setup()
{
    delay(2000);

    Params::loadPreferences(); // Load preferences from NVS

    // Register a CLI command with ID 'S' to set a variable
    // cli.addCommand('S', [](const char *arg)
    //                { cli.setVariable(arg); });

    // cli.addCommand('R', [](const char *arg)
    //                { Params::wipeSettings(); }); // Call wipeSettings when user sends 'R'

    stateMachine.begin(); // Start the state machine
}

void loop()
{
    vTaskDelay(portMAX_DELAY); // Suspend the task indefinitely
    // esp_log_set_vprintf
    // cli.run(); // Run the CLI
    // delay(100);
    // Empty if we want all logic in FreeRTOS tasks
}
