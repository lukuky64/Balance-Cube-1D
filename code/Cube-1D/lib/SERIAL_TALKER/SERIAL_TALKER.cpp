
#include "SERIAL_TALKER.hpp"

Serial_Talker::Serial_Talker(size_t bufferSize) : maxBufferSize(bufferSize)
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

void Serial_Talker::flushBuffer()
{
    return;
}
