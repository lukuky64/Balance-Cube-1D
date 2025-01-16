#pragma once

#include <SPI.h>
#include "Wire.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

// Forward declarations of SPIClass instances
extern SPIClass SPI_FSPI;
extern SPIClass SPI_HSPI;

// Forward declarations of I2C instances
extern TwoWire I2C0;

// Forward declarations of mutexes
extern SemaphoreHandle_t mutexFSPI;
extern SemaphoreHandle_t mutexHSPI;

// SPICOM struct declaration
struct SPICOM
{
    SPIClass *BUS;
    bool begun = false;
    SemaphoreHandle_t mutex; // Mutex for synchronizing SPI access
    uint32_t frequency;
};

// I2CCOM struct declaration
struct I2CCOM
{
    TwoWire *BUS;
    bool begun = false;
    uint32_t frequency;
};

// Forward declarations of SPICOM instances
extern SPICOM m_SPIComSensors;
extern SPICOM m_SPIComSD;

extern I2CCOM m_I2CComPeripherals;

// // Function to initialize mutexes
// void initialiseMutexes();
