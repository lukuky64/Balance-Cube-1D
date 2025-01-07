
#ifndef SERIAL_TALKER_HPP
#define SERIAL_TALKER_HPP

#include "Arduino.h"
#include "esp_log.h"

class Serial_Talker
{
public:
    Serial_Talker();
    ~Serial_Talker();
    bool checkStatus();
    bool begin();
    bool writeToBuffer(String dataString);
    void flushBuffer();
    bool writeBuffer(const char *buffer, size_t bufferIndex);

private:
    size_t maxBufferSize;
};

#endif // SERIAL_TALKER_HPP