
#include "Log.hpp"

Log::Log() : m_sdLog(false), m_serialLog(false), m_startTime(millis()), currentBufferPos(0)
{
    memset(charBuffer, 0, m_bufferSize); // Initialise buffer
}

Log::~Log()
{
}

bool Log::startNewLog()
{
    setStartTime();

    if (m_sdLog)
    {
        String startMsg = "Time(s)"; // Should always log time

        if (LOG_THETA)
        {
            startMsg += ",Theta(rad)";
        }
        if (LOG_THETA_DOT)
        {
            startMsg += ",theta_dot(rad/s)";
        }
        if (LOG_PHI)
        {
            startMsg += ",Phi(rad)";
        }
        if (LOG_PHI_DOT)
        {
            startMsg += ",phi_dot(rad/s)";
        }
        if (LOG_SETPOINT)
        {
            startMsg += ",setpoint(Nm)";
        }

        if (!m_sdTalker.createFile(startMsg, LOG_FILE_PREFIX))
        {
            ESP_LOGE(TAG, "Failed to create file on SD card!");
            return false;
        }
        else
        {
            ESP_LOGI(TAG, "Created file on SD card!");
            return true;
        }
    }

    if (m_serialLog)
    {
        m_serialTalker.begin();
        return true;
    }

    return false;
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

bool Log::logData(const float *data, int dataSize)
{
    // Ensure logging is set up
    if (!isLogSetup())
    {
        ESP_LOGE(TAG, "Logging not set up.");
        return false;
    }

    // Calculate timestamp in seconds
    float timeStamp = static_cast<float>(millis() - m_startTime) / 1000.0f;

    // Function to write formatted string to buffer with overflow protection
    auto writeToBuffer = [&](const char *format, float value, bool isLast = false) -> bool
    {
        // Estimate maximum required length for the formatted string
        // "%.3f," -> up to 12 characters including null terminator
        // Adjust maxFloatLen if necessary based on expected float range
        constexpr size_t maxFloatLen = 12;
        size_t remaining = m_bufferSize - currentBufferPos;

        // Check if there is enough space for the new data
        if (maxFloatLen > remaining)
        {
            // Flush current buffer
            if (!writeBufferAll())
            {
                ESP_LOGE(TAG, "Failed to flush buffer.");
                return false;
            }

            // Reset buffer pointers after flushing
            currentBufferPos = 0;
            remaining = m_bufferSize;
        }

        // Determine the actual character to append (comma or newline)
        char suffix = isLast ? '\n' : ',';

        // Write the formatted float to the buffer
        int written = snprintf(&charBuffer[currentBufferPos], remaining, format, value);
        if (written < 0)
        {
            ESP_LOGE(TAG, "Formatting error while writing float.");
            return false;
        }

        // Ensure we don't write beyond the buffer
        if (static_cast<size_t>(written) >= remaining)
        {
            ESP_LOGE(TAG, "Buffer overflow detected while writing float.");
            return false;
        }

        // Update buffer position
        currentBufferPos += written;
        remaining -= written;

        // Replace the trailing comma with the appropriate suffix
        if (currentBufferPos > 0)
        {
            charBuffer[currentBufferPos - 1] = suffix;
        }

        return true;
    };

    // Step 1: Write the timestamp with a trailing comma
    if (!writeToBuffer("%.3f,", timeStamp))
    {
        return false;
    }

    // Step 2: Write each data element with a trailing comma
    for (int i = 0; i < dataSize; ++i)
    {
        bool isLast = (i == dataSize - 1);
        if (!writeToBuffer("%.3f,", data[i], isLast))
        {
            return false;
        }
    }

    // If there are no data elements, ensure the timestamp line ends with a newline
    if (dataSize == 0 && currentBufferPos > 0)
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
            ESP_LOGE(TAG, "Failed to write to SD.");
            success = false;
        }
    }

    if (m_serialLog)
    {
        if (!m_serialTalker.writeBuffer(charBuffer, currentBufferPos))
        {
            ESP_LOGE(TAG, "Failed to write to Serial.");
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