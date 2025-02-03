
#include "Log.hpp"

Log::Log() : m_sdLog(false), m_serialLog(false), m_startTime(millis()), currentBufferPos(0)
{
    memset(charBuffer, 0, m_bufferSize); // Initialise buffer
}

Log::~Log()
{
}

/*****************************************************************************/
/*!
    @brief Start a new log file on the SD card or serial port. The file will be
    named sequentially with the prefix "LOG_FILE_PREFIX". The file will contain
    the time in seconds and the data specified by the LOG_ flags.

    @return True if the log file was created successfully, false otherwise.
*/
/*****************************************************************************/
bool Log::startNewLog()
{
    setStartTime();

    if (m_sdLog)
    {
        String startMsg = "Time(s)"; // Should always log time

        if (Params::LOG_THETA)
        {
            startMsg += ",Theta(rad)";
        }
        if (Params::LOG_THETA_DOT)
        {
            startMsg += ",theta_dot(rad/s)";
        }
        if (Params::LOG_PHI)
        {
            startMsg += ",Phi(rad)";
        }
        if (Params::LOG_PHI_DOT)
        {
            startMsg += ",phi_dot(rad/s)";
        }
        if (Params::LOG_SETPOINT)
        {
            startMsg += ",setpoint(A)";
        }

        if (!m_sdTalker.createFile(startMsg, Params::LOG_FILE_PREFIX))
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

/*****************************************************************************/
/*!
    @brief Sets the log flag for logging to the SD card.
*/
/*****************************************************************************/
void Log::selectLogSD()
{
    m_sdLog = true;
}

/*****************************************************************************/
/*!
    @brief Sets the log flag for logging to the serial port.
*/
/*****************************************************************************/
void Log::selectLogSerial()
{
    m_serialLog = true;
}

/*****************************************************************************/
/*!
    @brief Resets the start time for the log file.
*/
/*****************************************************************************/
void Log::setStartTime()
{
    m_startTime = millis();
}

/*****************************************************************************/
/*!
    @brief Logs the data to the SD card or serial port. The data is formatted
    as a comma-separated list of floats. The first float is the time in seconds
    since the log file was created.

    @param data Pointer to the data array to log.

    @param dataSize Number of elements in the data array.

    @return True if the data was logged successfully, false otherwise.
*/
/*****************************************************************************/
bool Log::logData(const float *data, int dataSize)
{
    // Ensure logging is set up
    if (!isLogSetup())
    {
        // ESP_LOGE(TAG, "Logging not set up.");
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

/*****************************************************************************/
/*!
    @brief Writes the buffer to the SD card or serial port.

    @return True if the buffer was written successfully, false otherwise.
*/
/*****************************************************************************/
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
            m_sdLog = false;
            success = false;
        }
    }

    if (m_serialLog)
    {
        if (!m_serialTalker.writeBuffer(charBuffer, currentBufferPos))
        {
            ESP_LOGE(TAG, "Failed to write to Serial.");
            m_serialLog = false;
            success = false;
        }
    }

    // Clear the buffer after writing
    memset(charBuffer, 0, m_bufferSize);
    currentBufferPos = 0;

    return success;
}

/*****************************************************************************/
/*!
    @brief Checks if logging is set up.

    @return True if logging is set up, false otherwise.
*/
/*****************************************************************************/
bool Log::isLogSetup()
{
    return (m_sdLog || m_serialLog);
}

/*****************************************************************************/
/*!
    @brief Forces the buffer to be written to the SD card or serial port.
*/
/*****************************************************************************/
void Log::forceFlush()
{
    writeBufferAll();
}