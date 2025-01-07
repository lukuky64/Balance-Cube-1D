#ifndef LOG_HPP
#define LOG_HPP

#include "Arduino.h"
#include "esp_log.h"
#include "Params.hpp"

#include "SD_TALKER.hpp"
#include "SERIAL_TALKER.hpp"

class Log
{
public:
    Log();
    ~Log();
    void selectLogSD();
    void selectLogSerial();
    // bool beginSerial();
    // bool beginSD();
    void startNewLog();

    bool log(int len);
    bool logData();
    void setStartTime();
    void forceFlush();

    bool writeBufferAll();

    bool isLogSetup();

    SD_Talker m_sdTalker;
    Serial_Talker m_serialTalker;

private:
    bool m_sdLog;
    bool m_serialLog;
    unsigned long m_startTime;

    static constexpr size_t m_bufferSize = 1024;
    char charBuffer[m_bufferSize];
    size_t currentBufferPos; // Track current position in buffer
};

#endif // LOG_HPP