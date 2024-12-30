#ifndef IMU_HPP
#define IMU_HPP

#include "Arduino.h"
#include <Adafruit_LSM6DSOX.h>

class IMU
{
public:
    IMU();
    void begin();
    void read();
    void print();
    float getAccelX();
    float getAccelY();
    float getAccelZ();
    float getGyroX();
    float getGyroY();
    float getGyroZ();

private:
    Adafruit_LSM6DSOX sox;
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
};

#endif // IMU_HPP