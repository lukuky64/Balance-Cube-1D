
#include "CONTROLLER.hpp"

Controller::Controller(Devices &devicesRef) : m_devicesRef(devicesRef),
                                              m_filters{Filter(0.1f, 1.0f, 1.0f, 0.0f), Filter(0.1f, 1.0f, 1.0f, 0.0f), Filter(0.1f, 1.0f, 1.0f, 0.0f), Filter(0.1f, 1.0f, 1.0f, 0.0f)},
                                              m_estimator(devicesRef, aquisition_dt_ms),
                                              m_controllableAngleThreshold(AngleThresh),
                                              m_wheel_J(wheel_J)
{
    m_target_tau_mutex = xSemaphoreCreateMutex();

    m_maxTau = m_devicesRef.m_bldc.getMaxTau();
}

Controller::~Controller()
{
    vSemaphoreDelete(m_target_tau_mutex);
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

// this needs to be called frequently
void Controller::updateData()
{
    // update a time step variable
    m_estimator.estimate();
    m_filters.filter_omega.update(m_estimator.getOmega());
    m_filters.filter_theta.update(m_estimator.getTheta());
}

void Controller::updateBalanceControl(float dt)
{
    m_devicesRef.m_bldc.moveTarget(linearRegulator(dt));
}

// this needs to be called fast as possible
void Controller::updateBLDC()
{
    m_devicesRef.m_bldc.loopFOC();
}

float (&Controller::getDataBuffer())[log_columns]
{
    // get data from the filters. Should match log_columns
    m_dataBuffer[0] = m_filters.filter_theta.getValue();
    m_dataBuffer[1] = m_filters.filter_omega.getValue();
    m_dataBuffer[2] = 0.0f;
    // probably want to add setpoint

    return m_dataBuffer;
}

float Controller::linearRegulator(float dt)
{
    m_traj_gen.generate(dt); // the purpose of this is to generate smoother control

    // get necessary data

    trajRefs refs = m_traj_gen.getRefs();
    float theta = m_filters.filter_theta.getValue();
    float omega = m_filters.filter_omega.getValue();

    // Calculate errors
    float error = refs.theta_r - theta;
    float error_dot = refs.omega_r - omega;

    // PID control law
    float u = (balance_Kp * error) + (balance_Kd * error_dot); // currently a PD controller

    // Feedforward control (where alpha_ref is the desired angular acceleration)
    u += refs.alpha_r;

    // Convert control input to torque
    float tau = u * m_wheel_J;

    // setTargetTau(tau);

    tau = std::clamp(tau, -m_maxTau, m_maxTau);

    return tau;
}

void Controller::setState()
{
    float currentAngle = m_filters.filter_theta.getValue();
    m_traj_gen.setTargetAngle(currentAngle, balanceAngle, balancePeriod);
}

// void Controller::setTargetTau(float tau)
// {
//     SemaphoreGuard guard(m_target_tau_mutex);
//     if (guard.acquired())
//     {
//         m_target_tau = tau;
//     }
// }

// float Controller::getTargetTau()
// {
//     SemaphoreGuard guard(m_target_tau_mutex);
//     if (guard.acquired())
//     {
//         return m_target_tau;
//     }
//     else
//     {
//         return 0.0;
//     }
// }