
#ifndef ROT_ENC_HPP
#define ROT_ENC_HPP

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

#endif // ROT_ENC_HPP