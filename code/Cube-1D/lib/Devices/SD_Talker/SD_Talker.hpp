#pragma once

#include "Arduino.h"
#include "esp_log.h"
#include <SD.h>
#include "Comms/Comms.hpp"
#include "SemaphoreGuard.hpp"

class SD_Talker
{
public:
    SD_Talker();
    ~SD_Talker();

#if DUMMY_SD

    bool checkStatus() { return true; }
    bool begin(uint8_t CS, SPICOM &SPI_BUS) { return true; }
    bool createFile(String StartMsg, String prefix) { return true; }
    bool writeToBuffer(String dataString) { return true; }
    void flushBuffer();
    bool isInitialized() { return true; }
    String createUniqueLogFile(String prefix) { return "true"; }
    bool createNestedDirectories(String prefix) { return true; }

#else
    bool checkStatus();
    bool begin(uint8_t CS, SPICOM &SPI_BUS);

    bool createFile(String StartMsg, String prefix);

    // bool writeToBuffer(String dataString);
    bool writeBuffer(const char *buffer, size_t bufferIndex);
    bool isInitialized();

    String createUniqueLogFile(String prefix);
    bool createNestedDirectories(String prefix);

private:
    File dataFile;
    String fileName;
    bool isFileOpen;
    String buffer;
    bool initialised;

    SPICOM *m_SPI_BUS = nullptr;

#endif
};