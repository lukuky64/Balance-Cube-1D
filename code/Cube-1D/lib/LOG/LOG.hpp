
#ifndef LOG_HPP
#define LOG_HPP

#include "Arduino.h"
#include "esp_log.h"

#include "SD_TALKER.hpp"
#include "SERIAL_TALKER.hpp"

class LOG
{
public:
    LOG(size_t bufferSize = 512);
    ~LOG();
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

    SD_TALKER m_sdTalker;
    SERIAL_TALKER m_serialTalker;

private:
    bool m_sdLog;
    bool m_serialLog;

    unsigned long m_startTime;
};

#endif // LOG_HPP