#ifndef COMMS_HPP
#define COMMS_HPP

#include "SPI.h"

struct SPICOM
{
    SPIClass *BUS;
    bool begun;
    SemaphoreHandle_t mutex; // Mutex for synchronizing SPI access
};

#endif // COMMS_HPP