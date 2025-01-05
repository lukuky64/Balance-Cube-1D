
#ifndef SERIAL_TALKER_HPP
#define SERIAL_TALKER_HPP

#include "Arduino.h"
#include "esp_log.h"

class Serial_Talker
{
public:
    Serial_Talker(size_t bufferSize = 512);
    ~Serial_Talker();
    bool checkStatus();
    bool begin();
    bool writeToBuffer(String dataString);
    void flushBuffer();

private:
    size_t maxBufferSize;
};

#endif // SERIAL_TALKER_HPP