#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "Arduino.h"
#include "esp_log.h"
#include "Devices.hpp"
#include "Filter.hpp"
#include "Estimator.hpp"

// create a set of filters for each device
struct FILTERS
{
    Filter filter_theta;
    Filter filter_omega;
    Filter filter_mag;
};

class Controller
{
public:
    Controller(Devices &devicesRef);

    ~Controller();

    void setup();
    bool checkStatus();
    bool controllableAngle();
    void updateFilters();
    void updateBalanceControl();
    void updateBLDC();

private:
    Devices &m_devicesRef;
    FILTERS m_filters;
    Estimator m_estimator;

    float m_controllableAngleThreshold;
};

#endif // CONTROLLER_HPP