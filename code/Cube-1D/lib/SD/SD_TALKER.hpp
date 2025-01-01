#ifndef SD_TALKER_HPP
#define SD_TALKER_HPP

#include "Arduino.h"
#include "esp_log.h"

class SD_TALKER
{
public:
    SD_TALKER();
    ~SD_TALKER();
    bool checkStatus();

private:
};

#endif // SD_TALKER_HPP