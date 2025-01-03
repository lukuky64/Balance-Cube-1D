#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "Arduino.h"
#include "esp_log.h"
#include "DEVICES.hpp"
#include "FILTER.hpp"
#include "Estimator.hpp"

// create a set of filters for each device
struct FILTERS
{
    FILTER filter_theta;
    FILTER filter_omega;

    FILTER filter_mag;
};

class CONTROLLER
{
public:
    CONTROLLER(DEVICES &devicesRef);

    ~CONTROLLER();

    void setup();
    bool checkStatus();
    bool controllableAngle();
    void updateFilters();
    void updateBalanceControl();
    void updateBLDC();

private:
    DEVICES &m_devicesRef;
    FILTERS m_filters;

    Estimator m_estimator;

    float m_controllableAngleThreshold;
};

#endif // CONTROLLER_HPP