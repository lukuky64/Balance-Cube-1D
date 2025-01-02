#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "Arduino.h"
#include "esp_log.h"
#include "DEVICES.hpp"
#include "FILTER.hpp"

// create a set of filters for each device
struct FILTERS
{
    FILTER filter_ax;
    FILTER filter_ay;
    FILTER filter_az;

    FILTER filter_gx;
    FILTER filter_gy;
    FILTER filter_gz;

    FILTER filter_mag;
};

class CONTROLLER
{
public:
    CONTROLLER(DEVICES &devicesRef);

    ~CONTROLLER();

    void setup();
    bool checkStatus();
    bool controlableAngle();
    bool updateFilters();

    // get the filtered values
    float getFilteredAccelX();
    float getFilteredAccelY();
    float getFilteredAccelZ();

    float getFilteredGyroX();
    float getFilteredGyroY();
    float getFilteredGyroZ();

    float getFilteredMag();

private:
    DEVICES &m_devicesRef;
    FILTERS m_filters;
};

#endif // CONTROLLER_HPP