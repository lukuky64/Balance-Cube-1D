
#ifndef LOG_HPP
#define LOG_HPP

#include "Arduino.h"
#include "esp_log.h"

#include "SD_TALKER.hpp"
#include "SERIAL_TALKER.hpp"

class LOG
{
public:
    LOG();
    ~LOG();
    // bool beginSerial();
    // bool beginSD();

    SD_TALKER m_sdTalker;
    SERIAL_TALKER m_serialTalker;

private:
    bool m_sdLog;
    bool m_serialLog;
};

#endif // LOG_HPP