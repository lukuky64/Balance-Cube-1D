
#include "Controller.hpp"

Controller::Controller(Devices &devicesRef) : m_devicesRef(devicesRef),
                                              m_filters{
                                                  Filter(0.25f, 0.1f, 100.0f, 0.0f), // Theta - don't trust initial values. Set 'P' high
                                                  Filter(0.25f, 0.1f, 1.0f, 0.0f),   // Omega
                                                  Filter(0.25f, 0.1f, 1.0f, 0.0f),   // Motor Theta
                                                  Filter(0.25f, 0.1f, 1.0f, 0.0f)},  // Motor Omega

                                              m_estimator(devicesRef, (Params::AQUISITION_MS / 1000.0f)),
                                              m_controlable(false),
                                              m_maxTau(0.0f)
{
    // m_target_tau_mutex = xSemaphoreCreateMutex();
    m_controllableMutex = xSemaphoreCreateMutex();
}

Controller::~Controller()
{
    // vSemaphoreDelete(m_target_tau_mutex);
    vSemaphoreDelete(m_controllableMutex);
}

bool Controller::setup()
{
    ESP_LOGI("Controller", "Setting up Controller!");

    bool succ = false;
    if (m_estimator.selectDevice())
        succ = m_estimator.calibrate();

    if (!succ)
        return false;

    // sample the readings of all the sensors
    succ &= setupFilters();

    m_maxTau = m_devicesRef.m_bldc.getMaxTau();

    return succ;
}

bool Controller::setupFilters()
{
    // averaging n samples for 0.5 seconds
    int n_samples = static_cast<int>(500 / (Params::AQUISITION_MS));

    for (int i = 0; i < n_samples; i++)
    {
        bool lastData = (i == n_samples - 1);

        m_estimator.estimate();
        m_filters.filter_theta.computeMeasurementVariance(m_estimator.getTheta(), lastData);
        m_filters.filter_omega.computeMeasurementVariance(m_estimator.getOmega(), lastData);
        m_filters.filter_motor_theta.computeMeasurementVariance(m_devicesRef.m_bldc.getTheta(), lastData);
        m_filters.filter_motor_omega.computeMeasurementVariance(m_devicesRef.m_bldc.getOmega(), lastData);

        vTaskDelay(pdMS_TO_TICKS(Params::AQUISITION_MS));
    }
    return true;
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
        m_controlable = (fabs(angle) < Params::ANGLE_THRESH);
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

void Controller::updateBalanceControl(float dt_ms)
{
    float dt = dt_ms / 1000.0f; // convert to seconds
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

float (&Controller::getDataBuffer())[MAX_LOG_COLUMNS]
{
    // get data from the filters. Should match log_columns

    if (Params::LOG_THETA)
    {
        m_dataBuffer[0] = m_filters.filter_theta.getValue();
    }
    if (Params::LOG_THETA_DOT)
    {
        m_dataBuffer[1] = m_filters.filter_omega.getValue();
    }
    if (Params::LOG_PHI)
    {
        m_dataBuffer[2] = m_filters.filter_motor_theta.getValue();
    }
    if (Params::LOG_PHI_DOT)
    {
        m_dataBuffer[3] = m_filters.filter_motor_omega.getValue();
    }
    if (Params::LOG_SETPOINT)
    {
        m_dataBuffer[4] = m_devicesRef.m_bldc.getTarget();
    }
    // probably want to add setpoint

    return m_dataBuffer;
}

float Controller::SoftClamp(float u)
{
    if (std::abs(u) > m_maxTau)
    {
        u = std::copysign(m_maxTau - std::exp(-std::abs(u) + m_maxTau) + 1, u); // +1 is a correction at the boundary to avoid a discontinuity
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
    float u = (Params::JERK_KP * error) + (Params::JERK_KD * error_dot); // currently a PD controller

    // Feedforward control (where alpha_ref is the desired angular acceleration)
    u += refs.alpha_r;

    // Convert control input to torque
    float u = u * Params::WHEEL_J;

    u = m_rateLimiter.limit(u, dt); // rate limit before clamping
    return SoftClamp(u);
}
#endif

// if using the min jerk controller, we must call this when entering the balance state
void Controller::setState()
{
    m_rateLimiter.reset();
    m_rateLimiter.setLimit(Params::RATE_LIMIT);

#if LQR
    m_lqrController.setGains(Params::LQR_K1, Params::LQR_K2, Params::LQR_K3, Params::LQR_K4);

#else
    float currentAngle = m_filters.filter_theta.getValue();
    m_minJerkController.setTargetAngle(currentAngle, Params::BALANCE_ANGLE, Params::BALANCE_PERIOD);
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