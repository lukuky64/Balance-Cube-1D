// COMMS.cpp

#include "Comms.hpp"

// Define SPI bus identifiers based on ESP32-S3
#define MY_FSPI FSPI // FSPI is defined as 0 for ESP32-S3
#define MY_HSPI HSPI // HSPI is defined as 1 for ESP32-S3

// Instantiate SPIClass objects (Definitions)
SPIClass SPI_FSPI(MY_FSPI);
SPIClass SPI_HSPI(MY_HSPI);

// Instantiate mutexes (Definitions)
SemaphoreHandle_t mutexFSPI = NULL;
SemaphoreHandle_t mutexHSPI = NULL;

// Instantiate SPICOM structs (Definitions)
SPICOM m_SPIComSensors = {&SPI_FSPI, false, NULL, 1000000}; // 1 MHz, (this is kinda low atm but the magnetic sensor is defaulted to this)
SPICOM m_SPIComSD = {&SPI_HSPI, false, NULL, 40000000};     // SD will use a different bus  // 40 MHz

// doing this externally at the moment
// // Function to initialize mutexes (Definitions)
// void initialiseMutexes()
// {
//     mutexFSPI = xSemaphoreCreateMutex();
//     if (mutexFSPI == NULL)
//     {
//         ESP_LOGI("COMMS", "Failed to create mutex for FSPI.");
//     }

//     mutexHSPI = xSemaphoreCreateMutex();
//     if (mutexHSPI == NULL)
//     {
//         ESP_LOGI("COMMS", "Failed to create mutex for HSPI.");
//     }

//     // Assign mutexes to SPICOM instances
//     m_SPIComSensors.mutex = mutexFSPI;
//     m_SPIComSD.mutex = mutexHSPI;
// }
