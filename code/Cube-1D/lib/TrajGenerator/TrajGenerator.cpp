#include "TrajGenerator.hpp"

TrajGenerator::TrajGenerator()
{
    m_state_mutex = xSemaphoreCreateMutex();
    m_ref_mutex = xSemaphoreCreateMutex();

    setTrajDyn(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    setRefs(0.0, 0.0, 0.0);
}

TrajGenerator::~TrajGenerator()
{
    vSemaphoreDelete(m_state_mutex);
    vSemaphoreDelete(m_ref_mutex);
}

// Generate step
void TrajGenerator::generate(float dt)
{

    SemaphoreGuard guard(m_state_mutex);
    if (guard.acquired())
    {

        // Update position (angle), velocity, acceleration, jerk, snap, and crackle
        m_stateDyn.pos += m_stateDyn.vel * dt + m_stateDyn.acc * pow(dt, 2) / 2.0 + m_stateDyn.jer * pow(dt, 3) / 6.0 + m_stateDyn.sna * pow(dt, 4) / 24.0 + m_stateDyn.cra * pow(dt, 5) / 120.0;
        m_stateDyn.vel += m_stateDyn.acc * dt + m_stateDyn.jer * pow(dt, 2) / 2.0 + m_stateDyn.sna * pow(dt, 3) / 6.0 + m_stateDyn.cra * pow(dt, 4) / 24.0;
        m_stateDyn.acc += m_stateDyn.jer * dt + m_stateDyn.sna * pow(dt, 2) / 2.0 + m_stateDyn.cra * pow(dt, 3) / 6.0;
        m_stateDyn.jer += m_stateDyn.sna * dt + m_stateDyn.cra * pow(dt, 2) / 2.0;
        m_stateDyn.sna += m_stateDyn.cra * dt;

        SemaphoreGuard guard(m_ref_mutex);
        if (guard.acquired())
        {
            // Orientation (angle of rotation about Z-axis)
            m_Ref.theta_r = m_stateDyn.pos; // Angle (position)
            m_Ref.omega_r = m_stateDyn.vel; // Reference angular velocity
            m_Ref.alpha_r = m_stateDyn.acc; // Reference angular acceleration
        }
    }
}

trajRefs TrajGenerator::getRefs()
{
    SemaphoreGuard guard(m_state_mutex);
    if (guard.acquired())
    {
        return m_Ref;
    }
    else
    {
        return trajRefs(0, 0, 0);
    }
}

void TrajGenerator::setTrajDyn(float pos, float vel, float acc, float jer, float sna, float cra)
{
    SemaphoreGuard guard(m_state_mutex);
    if (guard.acquired())
    {
        m_stateDyn.pos = pos;
        m_stateDyn.vel = vel;
        m_stateDyn.acc = acc;
        m_stateDyn.jer = jer;
        m_stateDyn.sna = sna;
        m_stateDyn.cra = cra;
    }
}

void TrajGenerator::setRefs(float theta_r, float omega_r, float alpha_r)
{
    SemaphoreGuard guard(m_ref_mutex);
    if (guard.acquired())
    {
        m_Ref.theta_r = theta_r;
        m_Ref.omega_r = omega_r;
        m_Ref.alpha_r = alpha_r;
    }
}

// void TrajGenerator::setTargetAngle(float angle)
// {
//     SemaphoreGuard guard(m_target_angle_mutex);
//     if (guard.acquired())
//     {
//         m_target_angle = angle;
//     }
// }