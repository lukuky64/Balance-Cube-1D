
#include "SERIAL_TALKER.hpp"

SERIAL_TALKER::SERIAL_TALKER(size_t bufferSize) : maxBufferSize(bufferSize)
{
}

SERIAL_TALKER::~SERIAL_TALKER()
{
}

bool SERIAL_TALKER::checkStatus()
{
    return true;
}

bool SERIAL_TALKER::begin()
{
    return true;
}
bool SERIAL_TALKER::writeToBuffer(String dataString)
{
    return true;
}

void SERIAL_TALKER::flushBuffer()
{
    return;
}
