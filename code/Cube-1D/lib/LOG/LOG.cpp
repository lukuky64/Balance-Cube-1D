
#include "LOG.hpp"

LOG::LOG(size_t bufferSize) : m_sdTalker(bufferSize), m_serialTalker(bufferSize), m_sdLog(false), m_serialLog(false), m_startTime(millis())
{
}

LOG::~LOG()
{
}

void LOG::startNewLog()
{
    setStartTime();

    if (m_sdLog)
    {
        if (!m_sdTalker.createFile("TEST", "/LOG"))
        {
            ESP_LOGE("LOG", "Failed to create file on SD card!");
        }
        else
        {
            ESP_LOGI("LOG", "Created file on SD card!");
        }
    }
}

void LOG::selectLogSD()
{
    m_sdLog = true;
}

void LOG::selectLogSerial()
{
    m_serialLog = true;
}

void LOG::setStartTime()
{
    m_startTime = millis();
}

bool LOG::logData()
{
    if (isLogSetup())
    {
        ESP_LOGI("LOG", "Logging Data");
        float timeStamp = static_cast<float>(millis() - m_startTime) / 1000.0f; // seconds
        String data = String(timeStamp, 3) + "\n";                              // Ensures three decimal places
        return log(data);
    }
    else
    {
        return false;
    }
}

bool LOG::log(String dataString)
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

bool LOG::forceFlush()
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

bool LOG::isLogSetup()
{
    return (m_sdLog || m_serialLog);
}