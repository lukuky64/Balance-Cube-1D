
#ifndef SERIAL_TALKER_HPP
#define SERIAL_TALKER_HPP

#include "Arduino.h"
#include "esp_log.h"

class SERIAL_TALKER
{
public:
    SERIAL_TALKER();
    ~SERIAL_TALKER();
    bool checkStatus();
    bool begin();

private:
};

#endif // SERIAL_TALKER_HPP