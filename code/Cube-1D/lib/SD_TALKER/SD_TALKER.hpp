#ifndef SD_TALKER_HPP
#define SD_TALKER_HPP

#include "Arduino.h"
#include "esp_log.h"
#include <SD.h>
#include "COMMS.hpp"
#include "SemaphoreGuard.hpp"

class SD_TALKER
{
public:
    SD_TALKER(size_t bufferSize = 512);
    ~SD_TALKER();

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

    bool writeToBuffer(String dataString);
    void flushBuffer();
    bool isInitialized();

    String createUniqueLogFile(String prefix);
    bool createNestedDirectories(String prefix);

private:
    File dataFile;
    String fileName;
    bool isFileOpen;
    String buffer;
    size_t maxBufferSize;
    bool initialised;

    SPICOM *m_SPI_BUS = nullptr;

#endif
};

#endif // SD_TALKER_HPP