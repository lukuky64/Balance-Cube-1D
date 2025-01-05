
#include "CONTROLLER.hpp"

Controller::Controller(Devices &devicesRef) : m_devicesRef(devicesRef),
                                              m_filters{Filter(0.1f, 1.0f, 1.0f, 0.0f), Filter(0.1f, 1.0f, 1.0f, 0.0f), Filter(0.1f, 1.0f, 1.0f, 0.0f)},
                                              m_estimator(devicesRef, aquisition_dt_ms),
                                              m_controllableAngleThreshold(AngleThresh)
{
}

Controller::~Controller()
{
}

void Controller::setup()
{
    ESP_LOGI("Controller", "Setting up Controller!");
    m_estimator.selectDevice();
}

bool Controller::checkStatus()
{
    return true;
}

bool Controller::controllableAngle()
{
    float angle = m_filters.filter_theta.getValue();

    ESP_LOGI("Controller", "Current angle: %f", angle);

    return (fabs(angle) < m_controllableAngleThreshold);
}

void Controller::updateFilters()
{
    m_estimator.estimate();
    m_filters.filter_omega.update(m_estimator.getOmega());
    m_filters.filter_theta.update(m_estimator.getTheta());
    m_filters.filter_mag.update(0.0f); // !!! MAG ENCODER NOT IMPLEMENTED YET !!!
}

void Controller::updateBalanceControl()
{
    // get errors, apply gains and all that jazz
}

void Controller::updateBLDC()
{
    // update the BLDC motor
}