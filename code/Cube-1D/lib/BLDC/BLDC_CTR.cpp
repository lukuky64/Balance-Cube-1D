#include "BLDC_CTR.hpp"

BLDC_CTR::BLDC_CTR()
{
}

BLDC_CTR::~BLDC_CTR()
{
}

void BLDC_CTR::begin()
{
    enableMotor(false); // Disable motor by default
}

bool BLDC_CTR::checkStatus()
{
    return true;
}

void BLDC_CTR::enableMotor(bool enable)
{
}