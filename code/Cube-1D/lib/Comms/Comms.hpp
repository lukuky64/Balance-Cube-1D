#ifndef COMMS_HPP
#define COMMS_HPP

#include <SPI.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

// Forward declarations of SPIClass instances
extern SPIClass SPI_FSPI;
extern SPIClass SPI_HSPI;

// Forward declarations of mutexes
extern SemaphoreHandle_t mutexFSPI;
extern SemaphoreHandle_t mutexHSPI;

// SPICOM struct declaration
struct SPICOM
{
    SPIClass *BUS;
    bool begun;
    SemaphoreHandle_t mutex; // Mutex for synchronizing SPI access
    uint32_t frequency;
};

// Forward declarations of SPICOM instances
extern SPICOM m_SPIComSensors;
extern SPICOM m_SPIComSD;

// // Function to initialize mutexes
// void initialiseMutexes();

#endif // COMMS_HPP
