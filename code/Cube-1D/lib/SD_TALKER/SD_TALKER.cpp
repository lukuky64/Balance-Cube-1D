#include "SD_TALKER.hpp"

#if DUMMY_SD
SD_TALKER::SD_TALKER(size_t bufferSize)
{
}

SD_TALKER::~SD_TALKER()
{
}

#else

SD_TALKER::SD_TALKER(size_t bufferSize) : isFileOpen(false), initialised(false), maxBufferSize(bufferSize)
{
}

SD_TALKER::~SD_TALKER()
{
    // Ensure the file is closed and buffer is flushed upon object destruction
    flushBuffer();
    if (isFileOpen)
    {
        dataFile.close();
    }
}

bool SD_TALKER::checkStatus()
{
    return true;
}

bool SD_TALKER::begin(uint8_t CS, SPICOM &SPI_BUS)
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

bool SD_TALKER::createNestedDirectories(String prefix)
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

bool SD_TALKER::createFile(String StartMsg, String prefix)
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
        }
    }

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

    return success;
}

bool SD_TALKER::writeToBuffer(String dataString)
{
    buffer += dataString; // Add newline for each entry

    // Check if buffer size exceeds the maximum size
    if (buffer.length() >= maxBufferSize)
    {
        flushBuffer(); // Write to SD card if buffer is full
    }

    return true;
}

void SD_TALKER::flushBuffer()
{
    if (isFileOpen && buffer.length() > 0)
    {
        dataFile.print(buffer); // Write buffer content to file
        buffer = "";            // Clear the buffer
        dataFile.flush();       // Ensure data is written to the card
    }
}

String SD_TALKER::createUniqueLogFile(String prefix)
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

bool SD_TALKER::isInitialized()
{
    return isFileOpen;
}

#endif
