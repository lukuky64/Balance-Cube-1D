
#include "SERIAL_TALKER.hpp"

Serial_Talker::Serial_Talker()
{
}

Serial_Talker::~Serial_Talker()
{
}

bool Serial_Talker::checkStatus()
{
    return true;
}

bool Serial_Talker::begin()
{
    return true;
}
bool Serial_Talker::writeToBuffer(String dataString)
{
    return true;
}

bool Serial_Talker::writeBuffer(const char *buffer, size_t bufferIndex)
{
    return true;
}

void Serial_Talker::flushBuffer()
{
    ESP_LOGI("SD_Talker", "Buffer Flushed!");
    return;
}
