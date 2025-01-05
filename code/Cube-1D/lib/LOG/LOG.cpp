
#include "Log.hpp"

Log::Log(size_t bufferSize) : m_sdTalker(bufferSize), m_serialTalker(bufferSize), m_sdLog(false), m_serialLog(false), m_startTime(millis())
{
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
        ESP_LOGI("Log", "Logging Data");
        float timeStamp = static_cast<float>(millis() - m_startTime) / 1000.0f; // seconds
        String data = String(timeStamp, 3) + "\n";                              // Ensures three decimal places
        return log(data);
    }
    else
    {
        return false;
    }
}

bool Log::log(String dataString)
{
    if (m_sdLog)
    {
        m_sdTalker.writeToBuffer(dataString);
    }

    if (m_serialLog)
    {
        m_serialTalker.writeToBuffer(dataString);
    }

    return true;
}

bool Log::forceFlush()
{

    if (m_sdLog)
    {
        m_sdTalker.flushBuffer();
    }

    if (m_serialLog)
    {
        m_serialTalker.flushBuffer();
    }

    return true;
}

bool Log::isLogSetup()
{
    return (m_sdLog || m_serialLog);
}