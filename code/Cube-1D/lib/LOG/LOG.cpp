
#include "Log.hpp"

Log::Log() : m_sdLog(false), m_serialLog(false), m_startTime(millis()), currentBufferPos(0)
{
    memset(charBuffer, 0, m_bufferSize); // Initialise buffer
}

Log::~Log()
{
}

void Log::startNewLog()
{
    setStartTime();

    if (m_sdLog)
    {
        if (!m_sdTalker.createFile("TEST", "/LOG"))
        {
            ESP_LOGE("Log", "Failed to create file on SD card!");
        }
        else
        {
            ESP_LOGI("Log", "Created file on SD card!");
        }
    }
}

void Log::selectLogSD()
{
    m_sdLog = true;
}

void Log::selectLogSerial()
{
    m_serialLog = true;
}

void Log::setStartTime()
{
    m_startTime = millis();
}

bool Log::logData()
{
    if (isLogSetup())
    {
        float timeStamp = static_cast<float>(millis() - m_startTime) / 1000.0f; // seconds

        // Prepare the data string
        int len = snprintf(charBuffer + currentBufferPos, m_bufferSize - currentBufferPos, "%.3f\n", timeStamp);

        if (len < 0)
        {
            ESP_LOGE("Log", "Failed to format log data.");
            return false;
        }

        // Check if adding this entry would exceed the buffer size
        if (currentBufferPos + len >= m_bufferSize)
        {
            // Buffer is full or would overflow, write current buffer
            if (!writeBufferAll())
            {
                ESP_LOGE("Log", "Failed to write buffer.");
                return false;
            }

            // Reset buffer position and write the new entry
            len = snprintf(charBuffer, m_bufferSize, "%.3f\n", timeStamp);
            if (len < 0 || len >= m_bufferSize)
            {
                ESP_LOGE("Log", "Log entry too large.");
                return false;
            }

            currentBufferPos = len;
        }
        else
        {
            // Append the new log entry to the buffer
            currentBufferPos += len;
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool Log::writeBufferAll()
{
    if (currentBufferPos == 0)
    {
        // Nothing to write
        return true;
    }

    bool success = true;

    if (m_sdLog)
    {
        if (!m_sdTalker.writeBuffer(charBuffer, currentBufferPos))
        {
            ESP_LOGE("Log", "Failed to write to SD.");
            success = false;
        }
    }

    if (m_serialLog)
    {
        if (!m_serialTalker.writeBuffer(charBuffer, currentBufferPos))
        {
            ESP_LOGE("Log", "Failed to write to Serial.");
            success = false;
        }
    }

    // Clear the buffer after writing
    memset(charBuffer, 0, m_bufferSize);
    currentBufferPos = 0;

    return success;
}

bool Log::isLogSetup()
{
    return (m_sdLog || m_serialLog);
}

void Log::forceFlush()
{
    writeBufferAll();
}