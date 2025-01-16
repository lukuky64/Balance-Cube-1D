
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
    // using ESP_LOG so we shouldn't need to do anything here
    return true;
}

// bool Serial_Talker::writeToBuffer(String dataString)
// {
//     return true;
// }

bool Serial_Talker::writeBuffer(const char *buffer, size_t bufferIndex)
{
    if (!buffer || bufferIndex == 0)
    {
        ESP_LOGW("Serial_Talker", "Attempting to write an empty or null buffer.");
        return false;
    }

    // Print out the buffer characters as an info log
    ESP_LOGI("Serial_Talker", "\n%.*s", bufferIndex, buffer);

    // Return true on successful write
    return true;
}

void Serial_Talker::flushBuffer()
{
    ESP_LOGI("SD_Talker", "Buffer Flushed!");
    return;
}
