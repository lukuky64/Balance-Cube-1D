#include "Estimator.hpp"

// Constructor
Estimator::Estimator(DEVICES &devicesRef, uint16_t dt) : m_devicesRef(devicesRef), m_imuSelected(false), m_rotEncSelected(false), m_dt(dt)
{
    m_theta_mutex = xSemaphoreCreateMutex();
    m_omega_mutex = xSemaphoreCreateMutex();

    // Estimator gains

    m_lds = 0.02;
    m_ldw = 50.0; // not used

    SemaphoreGuard guard(m_theta_mutex);
    if (guard.acquired())
    {
        SemaphoreGuard guard(m_omega_mutex);
        if (guard.acquired())
        {
            // Set initial orientation angle
            m_theta = 0.0;
            // Set initial angular velocity
            m_omega = 0.0;
        }
    }

    // Set initial angular velocity bias
    m_omegaBias = 0.0;

    selectDevice();
}

bool Estimator::selectDevice()
{
    // prioritise IMU over ROT_ENC
    if ((m_devicesRef.getStatus() & IMU_BIT) == IMU_BIT)
    {
        ESP_LOGI("ESTIMATOR", "IMU selected!");
        m_imuSelected = true;

        // Angular velocity bias calibration for IMU
        calibrate();
    }
    else if ((m_devicesRef.getStatus() & ROT_ENC_BIT) == ROT_ENC_BIT)

    {
        ESP_LOGI("ESTIMATOR", "ROT_ENC selected!");
        m_rotEncSelected = true;
    }
    else
    {
        ESP_LOGE("ESTIMATOR", "No device selected!");
    }

    return (m_imuSelected || m_rotEncSelected);
}

// Angular velocity bias calibration for IMU
void Estimator::calibrate()
{
    if (m_imuSelected)
    {
        // Calculate angular velocity bias by averaging n samples for 0.5 seconds
        int n = aquisitionFreq / 2;
        for (int i = 0; i < n; i++)
        {
            m_devicesRef.m_imu.update();
            m_omegaBias += m_devicesRef.m_imu.getGyroY() / n;
            vTaskDelay(pdMS_TO_TICKS(m_dt));
        }
    }
}

void Estimator::estimateIMU()
{
    // Update latest readings
    m_devicesRef.m_imu.update();

    // Get angular velocity from IMU gyroscope data
    float omega_measured = m_devicesRef.m_imu.getGyroY();

    // Predict step
    predict(omega_measured);

    // Get linear acceleration from IMU accelerometer data
    float ax = m_devicesRef.m_imu.getAccelX(); // Assuming X-axis
    float ay = m_devicesRef.m_imu.getAccelY(); // Assuming Y-axis

    // Normalize linear acceleration
    float a_norm = sqrt(ax * ax + ay * ay);
    if (a_norm == 0)
        a_norm = 1; // Prevent division by zero

    ax /= a_norm;
    ay /= a_norm;

    // Correct step
    correct(ax, ay);

    WrapTheta(); // wrap theta to [-π, π]
}

// wrap theta to [-π, π]
void Estimator::WrapTheta()
{
    SemaphoreGuard guard(m_theta_mutex);
    if (guard.acquired())
    {
        if (m_theta > PI)
            m_theta -= 2 * PI;
        else if (m_theta < -PI)
            m_theta += 2 * PI;
    }
}

// Estimate step. This is the main function that should be called periodically
void Estimator::estimate()
{
    if (m_imuSelected)
    {
        estimateIMU();
    }

    if (m_rotEncSelected)
    {
        estimateROT_ENC();
    }

    // !!! need to somehow fuse these two, if they are both selected
}

void Estimator::estimateROT_ENC()
{
    // Example: Read encoder value and convert to angle
    float encoder_angle = m_devicesRef.m_rotEnc.getAngle();
    // Implement prediction and correction based on encoder data
}

// Predict step
void Estimator::predict(float omega_measured)
{
    SemaphoreGuard guard(m_theta_mutex);
    if (guard.acquired())
    {
        SemaphoreGuard guard(m_omega_mutex);
        if (guard.acquired())
        {
            // Correct the measured angular velocity with bias
            float omega_corrected = omega_measured - m_omegaBias;
            // Update orientation
            m_theta += omega_corrected * m_dt;
            // Optionally, you can also update omega if you want to track it
            m_omega = omega_corrected;
        }
    }
}

// Correct step
void Estimator::correct(float ax, float ay)
{
    SemaphoreGuard guard(m_theta_mutex);
    if (guard.acquired())
    {
        // Assuming ax and ay are normalized
        // Calculate the expected gravity direction
        float gravity_est_x = cos(m_theta);
        float gravity_est_y = sin(m_theta);

        // Compute error between measured and estimated gravity
        float error_x = ax - gravity_est_x;
        float error_y = ay - gravity_est_y;

        // Compute error angle (simplified)
        float error_theta = atan2(error_y, error_x);

        // Apply correction
        m_theta += m_lds * error_theta * m_dt;
    }
}

float Estimator::getTheta()
{
    SemaphoreGuard guard(m_theta_mutex);
    if (guard.acquired())
    {
        return m_theta;
    }
    else
    {
        return 0.0;
    }
}

float Estimator::getOmega()
{
    SemaphoreGuard guard(m_omega_mutex);
    if (guard.acquired())
    {
        return m_omega;
    }
    else
    {
        return 0.0;
    }
}