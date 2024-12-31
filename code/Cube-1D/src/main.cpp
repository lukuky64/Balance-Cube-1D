#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "INDICATORS.hpp"

#include "esp_log.h"

#include "STATEMACHINE.hpp"

// Create an instance of the STATEMACHINE class
STATEMACHINE stateMachine;

// Create an instance of the INDICATORS class
INDICATORS indicators;

void criticalErrorTask(void *pvParameters)
{
    while (true)
    {
        indicators.showCriticalError();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void warningTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(1000)); // Initial offset of 1 second
    while (true)
    {
        indicators.showWarning();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void successTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(2000)); // Initial offset of 2 seconds
    while (true)
    {
        indicators.showSuccess();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void allGoodTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(3000)); // Initial offset of 3 seconds
    while (true)
    {
        indicators.showAllGood();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup()
{
    // INITIALISE_DBG(115200);
    // DBG("Setup completed.");

    // Set up the buzzer and RGB LED
    indicators.setupRGBLed(39, 40, 41);

    // Create FreeRTOS tasks
    // xTaskCreate(criticalErrorTask, "CriticalErrorTask", 4096, NULL, 1, NULL);
    // xTaskCreate(successTask, "SuccessTask", 4096, NULL, 1, NULL);
    // xTaskCreate(allGoodTask, "AllGoodTask", 4096, NULL, 1, NULL);
    xTaskCreate(warningTask, "WarningTask", 4096, NULL, 1, NULL);
}

void loop()
{
    // Remains empty if you want all logic in FreeRTOS tasks
}
