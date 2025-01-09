#include "SD_Talker.hpp"

#if DUMMY_SD
SD_Talker::SD_Talker()
{
}

SD_Talker::~SD_Talker()
{
}

#else

SD_Talker::SD_Talker() : isFileOpen(false), initialised(false)
{
}

SD_Talker::~SD_Talker()
{
    // Ensure the file is closed and buffer is flushed upon object destruction
    // flushBuffer();
    if (isFileOpen)
    {
        SemaphoreGuard guard(m_SPI_BUS->mutex);
        if (guard.acquired())
        {
            dataFile.close();
        }
    }
}

// !!! this doesn't work as intended
bool SD_Talker::checkStatus()
{
    SemaphoreGuard guard(m_SPI_BUS->mutex);
    if (guard.acquired())
    {
        // Retrieve the card type
        uint8_t cardType = SD.cardType();

        if (cardType == CARD_NONE)
        {
            ESP_LOGI("SD_Talker", "SD card not connected.");
            return false;
        }
        else
        {
            ESP_LOGI("SD_Talker", "SD card connected. Type: %d", cardType);
            return true;
        }
    }
    else
    {
        ESP_LOGI("SD_Talker", "Failed to acquire SD mutex for status check.");
        return false;
    }
}

bool SD_Talker::begin(uint8_t CS, SPICOM &SPI_BUS)
{
    m_SPI_BUS = &SPI_BUS;

    SemaphoreGuard guard(m_SPI_BUS->mutex);
    if (guard.acquired())
    {
        // See if the card is present and can be initialized:
        if (!SD.begin(CS, *m_SPI_BUS->BUS))
        {
            initialised = false;
        }
        else
        {
            initialised = true;
        }
    }
    return initialised;
}

bool SD_Talker::createNestedDirectories(String prefix)
{
    bool success = true;

    // Count how many slashes are in the prefix
    uint8_t count = 0;
    for (int i = 0; i < prefix.length(); i++)
    {
        if (prefix.charAt(i) == '/')
        {
            count++;
        }
    }

    if (count > 0)
    {
        int start = 0;
        if (prefix.charAt(0) == '/')
        {
            start = 1; // Skip the leading slash
        }

        // Start from 'start' instead of 0 when taking the substring
        int pos = prefix.indexOf('/', start);

        {
            SemaphoreGuard guard(m_SPI_BUS->mutex);
            if (guard.acquired())
            {
                while (pos != -1)
                {
                    String folder = prefix.substring(start, pos);

                    if (!SD.exists(folder))
                    {
                        if (!SD.mkdir(folder))
                        {
                            success = false;
                            break;
                        }
                        else
                        {
                        }
                    }
                    pos = prefix.indexOf('/', pos + 1);
                }
            }
            else
            {
                success = false;
            }
        }
    }

    return success;
}

bool SD_Talker::createFile(String StartMsg, String prefix)
{
    bool success = false;

    // first lets make sure we have the correct folder
    createNestedDirectories(prefix);

    fileName = createUniqueLogFile(prefix);

    {
        SemaphoreGuard guard(m_SPI_BUS->mutex);
        if (guard.acquired())
        {
            dataFile = SD.open(fileName.c_str(), FILE_WRITE);
            if (dataFile)
            {
                dataFile.println(StartMsg);
                dataFile.flush();
                isFileOpen = true;
                success = true;
            }
            else
            {
                success = false;
            }
        }
    }

    return success;
}

// bool SD_Talker::writeToBuffer(String dataString)
// {
//     buffer += dataString; // Add newline for each entry

//     // Check if buffer size exceeds the maximum size
//     if (buffer.length() >= maxBufferSize)
//     {
//         flushBuffer(); // Write to SD card if buffer is full
//     }

//     return true;
// }

bool SD_Talker::writeBuffer(const char *buffer, size_t bufferIndex)
{
    if (isFileOpen)
    {
        SemaphoreGuard guard(m_SPI_BUS->mutex);
        if (guard.acquired())
        {
            size_t bytesWritten = dataFile.write((const uint8_t *)buffer, bufferIndex);
            dataFile.flush();
            if (bytesWritten != bufferIndex)
            {
                ESP_LOGE("SD_Talker", "Failed to write all bytes to SD card.");
                return false;
            }
            else
            {
                ESP_LOGI("SD_Talker", "Successfully wrote %d bytes to SD card.", bytesWritten);
                return true;
            }
        }
        else
        {
            ESP_LOGE("SD_Talker", "Failed to acquire SD mutex for writing.");
            return false;
        }
    }
    else
    {
        ESP_LOGE("SD_Talker", "Attempted to write to SD card, but file is not open.");
        return false;
    }
}

String SD_Talker::createUniqueLogFile(String prefix)
{
    String uniqueFileName;
    uint32_t currentLogIndex = 0;

    // Generate a unique file name
    {
        SemaphoreGuard guard(m_SPI_BUS->mutex);
        if (guard.acquired())
        {
            do
            {
                uniqueFileName = String(prefix) + "_" + String(currentLogIndex++) + ".csv";
            } while (SD.exists(uniqueFileName.c_str())); // Check if the file already exists}

            return uniqueFileName;
        }
    }
    return "";
}

bool SD_Talker::isInitialized()
{
    return isFileOpen;
}

#endif
