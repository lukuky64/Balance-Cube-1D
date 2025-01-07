
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
        if (!m_sdTalker.createFile(log_header, log_file_prefix))
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

bool Log::logData(float *data, int dataSize)
{
    // Ensure logging is set up
    if (!isLogSetup())
    {
        return false;
    }

    // Calculate time stamp in seconds
    float timeStamp = static_cast<float>(millis() - m_startTime) / 1000.0f;

    // 1) Write the timestamp with a trailing comma
    int remaining = m_bufferSize - currentBufferPos;
    int len = snprintf(&charBuffer[currentBufferPos], remaining, "%.3f,", timeStamp);
    if (len < 0 || len >= remaining)
    {
        ESP_LOGE("Log", "Failed to format timestamp or buffer overflow.");
        return false;
    }
    currentBufferPos += len;
    remaining -= len;

    // 2) Handle each data element
    for (int i = 0; i < dataSize; ++i)
    {
        // Estimate maximum size needed for a single float + comma (e.g., "-1234.567,\0")
        // This is conservative—adjust for your max float format width
        const int maxFloatLen = 12;

        // If insufficient space remains, flush then retry once
        if (maxFloatLen > remaining)
        {
            // Attempt to flush buffer
            if (!writeBufferAll())
            {
                ESP_LOGE("Log", "Failed to write buffer before retrying data element.");
                return false;
            }

            // Reset buffer pointers
            currentBufferPos = 0;
            remaining = m_bufferSize;
        }

        // Now safe to write the float (with snprintf)
        len = snprintf(&charBuffer[currentBufferPos], remaining, "%.3f,", data[i]);
        if (len < 0 || len >= remaining)
        {
            ESP_LOGE("Log", "Data element too large or formatting error.");
            return false;
        }
        currentBufferPos += len;
        remaining -= len;
    }

    // 3) If we wrote at least one float, replace trailing comma with newline
    //    (if dataSize == 0, we only have the timestamp comma)
    if (currentBufferPos > 0)
    {
        charBuffer[currentBufferPos - 1] = '\n';
    }

    return true;
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