#include "LQRController.hpp"

LQRController::LQRController() : m_K{0, 0, 0, 0}
{
    //
}

LQRController::~LQRController()
{
    //
}

void LQRController::setGains(float k1, float k2, float k3, float k4)
{
    m_K.k1 = k1;
    m_K.k2 = k2;
    m_K.k3 = k3;
    m_K.k4 = k4;
}

// LQR Controller
float LQRController::generate(float theta, float theta_dot, float phi, float phi_dot)
{
    // x = [theta, theta_dot, phi, phi_dot]
    // Compute the control input: u = -K * x

    // WrapAngle(phi);
    return -(m_K.k1 * theta +
             m_K.k2 * theta_dot +
             m_K.k3 * phi +
             m_K.k4 * phi_dot);
}

// keeps the angle between 0 and 2*PI
void LQRController::WrapAngle(float &angle)
{
    while (angle < 0)
        angle += 2 * PI;
    while (angle >= 2 * PI)
        angle -= 2 * PI;
}