#ifndef LOG_HPP
#define LOG_HPP

#include "Arduino.h"
#include "esp_log.h"

#include "SD_TALKER.hpp"
#include "SERIAL_TALKER.hpp"

class Log
{
public:
    Log(size_t bufferSize = 512);
    ~Log();
    void selectLogSD();
    void selectLogSerial();
    // bool beginSerial();
    // bool beginSD();
    void startNewLog();

    bool log(String dataString);
    bool logData();
    bool forceFlush();
    void setStartTime();

    bool isLogSetup();

    SD_Talker m_sdTalker;
    Serial_Talker m_serialTalker;

private:
    bool m_sdLog;
    bool m_serialLog;

    unsigned long m_startTime;
};

#endif // LOG_HPP