#include "TrajGenerator.hpp"

TrajGenerator::TrajGenerator()
{
    m_coeff_mutex = xSemaphoreCreateMutex();
    m_ref_mutex = xSemaphoreCreateMutex();

    setRefs(0.0, 0.0, 0.0);
}

TrajGenerator::~TrajGenerator()
{
    vSemaphoreDelete(m_coeff_mutex);
    vSemaphoreDelete(m_ref_mutex);
}

// Generate step
void TrajGenerator::generate(float dt)
{
    // Update elapsed time
    m_elapsed += dt;
    if (m_elapsed > m_totalTime)
        m_elapsed = m_totalTime; // clamp to avoid overshoot

    // Evaluate polynomial at current time
    float t = m_elapsed;
    float t2 = t * t;
    float t3 = t2 * t;
    float t4 = t3 * t;
    float t5 = t4 * t;

    SemaphoreGuard guard1(m_coeff_mutex);
    SemaphoreGuard guard2(m_ref_mutex);
    if (guard1.acquired() && guard2.acquired())
    {
        // Position (angle)
        m_Ref.theta_r = m_coeffs.a0 + m_coeffs.a1 * t + m_coeffs.a2 * t2 + m_coeffs.a3 * t3 + m_coeffs.a4 * t4 + m_coeffs.a5 * t5;
        // Velocity (angular)
        m_Ref.omega_r = (m_coeffs.a1) + (2.0f * m_coeffs.a2) * t + (3.0f * m_coeffs.a3) * t2 + (4.0f * m_coeffs.a4) * t3 + (5.0f * m_coeffs.a5) * t4;
        // Acceleration (angular)
        m_Ref.alpha_r = (2.0f * m_coeffs.a2) + (6.0f * m_coeffs.a3) * t + (12.0f * m_coeffs.a4) * t2 + (20.0f * m_coeffs.a5) * t3;
    }
}

trajRefs TrajGenerator::getRefs()
{
    SemaphoreGuard guard(m_ref_mutex);
    if (guard.acquired())
    {
        return m_Ref;
    }
    else
    {
        return trajRefs(0, 0, 0);
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

void TrajGenerator::setTargetAngle(float startAngle, float targetAngle, float totalTime)
{
    m_totalTime = totalTime;
    m_elapsed = 0.0f; // reset time

    // Boundary conditions:
    // theta(0) = startAngle
    // theta(T) = targetAngle
    // theta'(0) = 0,    theta'(T) = 0
    // theta''(0) = 0,   theta''(T) = 0

    float T = totalTime;
    float T2 = T * T;
    float T3 = T2 * T;
    float T4 = T3 * T;
    float T5 = T4 * T;

    float theta0 = startAngle;
    float thetaf = targetAngle;

    // Let the polynomial be:
    // theta(t) = a0 + a1 t + a2 t^2 + a3 t^3 + a4 t^4 + a5 t^5
    //
    // We know:
    // theta(0) = a0 = theta0
    // theta(T) = a0 + a1 T + a2 T^2 + a3 T^3 + a4 T^4 + a5 T^5 = thetaf
    // and so on for derivatives = 0 at t=0 and t=T

    SemaphoreGuard guard(m_coeff_mutex);
    if (guard.acquired())
    {
        m_coeffs.a0 = theta0; // from theta(0) = theta0

        // We have 5 unknowns left: a1..a5
        // The system of equations can be solved as:
        //   a1*T + a2*T^2 + a3*T^3 + a4*T^4 + a5*T^5 = thetaf - theta0
        //   derivative constraints at t=0 and t=T => a1, a2, a3, a4, a5

        // For simplicity, use a well-known closed form solution:

        // Common approach (two more references for the derivative constraints at T):
        //   (1) theta'(T) = 0 => a1 + 2*a2*T + 3*a3*T^2 + 4*a4*T^3 + 5*a5*T^4 = 0
        //   (2) theta''(T) = 0 => 2*a2 + 6*a3*T + 12*a4*T^2 + 20*a5*T^3 = 0
        //   (3) theta'(0) = a1 = 0
        //   (4) theta''(0) = 2*a2 = 0

        // So from the above:
        //   a1 = 0
        //   a2 = 0
        //
        // Then only a3, a4, a5 remain. The position constraint at T => a3*T^3 + a4*T^4 + a5*T^5 = (thetaf - theta0)
        // and the velocity and acceleration constraints at T give 2 more equations.
        //
        // That system yields:
        float delta = thetaf - theta0;
        // a1 = 0
        // a2 = 0
        // Solve for a3, a4, a5:
        m_coeffs.a3 = 10.0f * delta / T3;
        m_coeffs.a4 = -15.0f * delta / T4;
        m_coeffs.a5 = 6.0f * delta / T5;
    }
}
