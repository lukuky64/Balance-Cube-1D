
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

    SD_TALKER m_sdTalker;
    SERIAL_TALKER m_serialTalker;

private:
    
};

#endif // LOG_HPP