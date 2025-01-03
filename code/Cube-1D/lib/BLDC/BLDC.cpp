#include "BLDC.hpp"

BLDC::BLDC()
{
}

BLDC::~BLDC()
{
}

void BLDC::begin()
{
    enableMotor(false); // Disable motor by default
}

bool BLDC::checkStatus()
{
    return true;
}

void BLDC::enableMotor(bool enable)
{
}