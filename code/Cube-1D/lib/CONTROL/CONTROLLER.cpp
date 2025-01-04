
#include "CONTROLLER.hpp"

CONTROLLER::CONTROLLER(DEVICES &devicesRef) : m_devicesRef(devicesRef),
                                              m_filters{FILTER(0.1f, 1.0f, 1.0f, 0.0f), FILTER(0.1f, 1.0f, 1.0f, 0.0f), FILTER(0.1f, 1.0f, 1.0f, 0.0f)},
                                              m_estimator(devicesRef, aquisition_dt_ms),
                                              m_controllableAngleThreshold(AngleThresh)
{
}

CONTROLLER::~CONTROLLER()
{
}

void CONTROLLER::setup()
{
    ESP_LOGI("CONTROLLER", "Setting up controller!");
    m_estimator.selectDevice();
}

bool CONTROLLER::checkStatus()
{
    return true;
}

bool CONTROLLER::controllableAngle()
{
    float angle = m_filters.filter_theta.getValue();

    ESP_LOGI("CONTROLLER", "Current angle: %f", angle);

    return (fabs(angle) < m_controllableAngleThreshold);
}

void CONTROLLER::updateFilters()
{
    m_estimator.estimate();
    m_filters.filter_omega.update(m_estimator.getOmega());
    m_filters.filter_theta.update(m_estimator.getTheta());
    m_filters.filter_mag.update(0.0f); // !!! MAG ENCODER NOT IMPLEMENTED YET !!!
}

void CONTROLLER::updateBalanceControl()
{
    // get errors, apply gains and all that jazz
}

void CONTROLLER::updateBLDC()
{
    // update the BLDC motor
}