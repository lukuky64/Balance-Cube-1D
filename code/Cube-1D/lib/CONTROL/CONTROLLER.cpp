
#include "CONTROLLER.hpp"

CONTROLLER::CONTROLLER(DEVICES &devicesRef) : m_devicesRef(devicesRef),
                                              m_filters{FILTER(0.1f, 1.0f, 1.0f, 0.0f), FILTER(0.1f, 1.0f, 1.0f, 0.0f),
                                                        FILTER(0.1f, 1.0f, 1.0f, 0.0f), FILTER(0.1f, 1.0f, 1.0f, 0.0f),
                                                        FILTER(0.1f, 1.0f, 1.0f, 0.0f), FILTER(0.1f, 1.0f, 1.0f, 0.0f),
                                                        FILTER(0.1f, 1.0f, 1.0f, 0.0f)}
{
}

CONTROLLER::~CONTROLLER()
{
}

void CONTROLLER::setup()
{
}

bool CONTROLLER::checkStatus()
{
    return true;
}

bool CONTROLLER::controlableAngle()
{
    return true;
}

bool CONTROLLER::updateFilters()
{
    m_filters.filter_ax.update(m_devicesRef.m_imu.getAccelX());
    m_filters.filter_ay.update(m_devicesRef.m_imu.getAccelY());
    m_filters.filter_az.update(m_devicesRef.m_imu.getAccelZ());

    m_filters.filter_gx.update(m_devicesRef.m_imu.getGyroX());
    m_filters.filter_gy.update(m_devicesRef.m_imu.getGyroY());
    m_filters.filter_gz.update(m_devicesRef.m_imu.getGyroZ());

    m_filters.filter_mag.update(0.0f); // !!! MAG ENCODER NOT IMPLEMENTED YET !!!

    return true;
}

// get the filtered values
float CONTROLLER::getFilteredAccelX()
{
    return m_filters.filter_ax.getValue();
}

float CONTROLLER::getFilteredAccelY()
{
    return m_filters.filter_ay.getValue();
}

float CONTROLLER::getFilteredAccelZ()
{
    return m_filters.filter_az.getValue();
}

float CONTROLLER::getFilteredGyroX()
{
    return m_filters.filter_gx.getValue();
}

float CONTROLLER::getFilteredGyroY()
{
    return m_filters.filter_gy.getValue();
}

float CONTROLLER::getFilteredGyroZ()
{
    return m_filters.filter_gz.getValue();
}

float CONTROLLER::getFilteredMag()
{
    return m_filters.filter_mag.getValue();
}