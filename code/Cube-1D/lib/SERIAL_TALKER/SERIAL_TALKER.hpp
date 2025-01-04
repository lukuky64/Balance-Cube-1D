
#ifndef SERIAL_TALKER_HPP
#define SERIAL_TALKER_HPP

#include "Arduino.h"
#include "esp_log.h"

class SERIAL_TALKER
{
public:
    SERIAL_TALKER(size_t bufferSize = 512);
    ~SERIAL_TALKER();
    bool checkStatus();
    bool begin();
    bool writeToBuffer(String dataString);
    void flushBuffer();

private:
    size_t maxBufferSize;
};

#endif // SERIAL_TALKER_HPP