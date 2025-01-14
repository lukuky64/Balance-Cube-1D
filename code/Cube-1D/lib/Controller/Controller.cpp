
#include "Controller.hpp"

Controller::Controller(Devices &devicesRef) : m_devicesRef(devicesRef),
                                              m_filters{Filter(0.5f, 0.5f, 1.0f, 0.0f), Filter(0.5f, 0.5f, 1.0f, 0.0f), Filter(0.5f, 0.5f, 1.0f, 0.0f), Filter(0.5f, 0.5f, 1.0f, 0.0f)},
                                              m_estimator(devicesRef, aquisition_dt_ms),
                                              m_controlable(false)
{
    // m_target_tau_mutex = xSemaphoreCreateMutex();
    m_controllableMutex = xSemaphoreCreateMutex();
    m_maxTau = m_devicesRef.m_bldc.getMaxTau();
}

Controller::~Controller()
{
    // vSemaphoreDelete(m_target_tau_mutex);
    vSemaphoreDelete(m_controllableMutex);
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

bool Controller::getControllable()
{
    SemaphoreGuard guard(m_controllableMutex);
    if (guard.acquired())
    {
        return m_controlable;
    }
    else
    {
        return false;
    }
}

void Controller::updateControlability()
{
    SemaphoreGuard guard(m_controllableMutex);
    if (guard.acquired())
    {
        float angle = m_filters.filter_theta.getValue();
        // ESP_LOGI("Controller", "Current angle: %f", angle);
        m_controlable = (fabs(angle) < AngleThresh);
    }
}

// this needs to be called frequently
void Controller::updateData()
{
    // update a time step variable
    m_estimator.estimate();

    // update the filters
    m_filters.filter_theta.update(m_estimator.getTheta()); // we can also add the control effort here
    m_filters.filter_omega.update(m_estimator.getOmega());

    m_filters.filter_motor_theta.update(m_devicesRef.m_bldc.getTheta());
    m_filters.filter_motor_omega.update(m_devicesRef.m_bldc.getOmega());

    updateControlability();
}

void Controller::updateBalanceControl(float dt)
{
#if LQR
    m_devicesRef.m_bldc.moveTarget(LQRegulator(dt));
#else
    m_devicesRef.m_bldc.moveTarget(linearRegulator(dt));
#endif
}

// this needs to be called as fast as possible
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

float Controller::SoftClamp(float u)
{
    if (std::abs(u) > m_maxTau)
    {
        u = std::copysign(m_maxTau - std::exp(-std::abs(u) + m_maxTau), u);
    }

    return u;
}

#if LQR
float Controller::LQRegulator(float dt)
{
    // Read the current state (assume these are updated elsewhere)
    float theta = m_filters.filter_theta.getValue();         // Position (angle)
    float theta_dot = m_filters.filter_omega.getValue();     // Velocity (angular)
    float phi = m_filters.filter_motor_theta.getValue();     // Reaction wheel angle
    float phi_dot = m_filters.filter_motor_omega.getValue(); // Reaction wheel angular velocity

    float u = m_lqrController.generate(theta, theta_dot, phi, phi_dot);
    u = m_rateLimiter.limit(u, dt); // rate limit before clamping
    return SoftClamp(u);
}

#else
float Controller::linearRegulator(float dt)
{
    trajRefs refs = m_minJerkController.generate(dt); // the purpose of this is to generate smoother control

    float theta = m_filters.filter_theta.getValue();
    float omega = m_filters.filter_omega.getValue();

    // Calculate errors
    float error = refs.theta_r - theta;
    float error_dot = refs.omega_r - omega;

    // PID control law
    float u = (jerk_Kp * error) + (jerk_Kd * error_dot); // currently a PD controller

    // Feedforward control (where alpha_ref is the desired angular acceleration)
    u += refs.alpha_r;

    // Convert control input to torque
    float u = u * wheel_J;

    u = m_rateLimiter.limit(u, dt); // rate limit before clamping
    return SoftClamp(u);
}
#endif

// if using the min jerk controller, we must call this when entering the balance state
void Controller::setState()
{
    m_rateLimiter.reset();
    m_rateLimiter.setLimit(RATE_LIMIT);

#if LQR
    m_lqrController.setGains(LQR_K1, LQR_K2, LQR_K3, LQR_K4);

#else
    float currentAngle = m_filters.filter_theta.getValue();
    m_minJerkController.setTargetAngle(currentAngle, balanceAngle, balancePeriod);
#endif
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