#pragma once

#include "Arduino.h"

class Rot_Enc
{
public:
    Rot_Enc();
    ~Rot_Enc();
    bool checkStatus();
    float getAngle();

private:
};