#include "Estimator.hpp"

// Constructor
Estimator::Estimator(Devices &devicesRef, uint16_t dt)
    : m_devicesRef(devicesRef),
      m_imuSelected(false),
      m_rotEncSelected(false),
      m_dt(dt),
      m_lds(0.2),
      m_omegaBias(0.0),
      m_startAngle(0.0)
{
    m_theta_mutex = xSemaphoreCreateMutex();
    m_omega_mutex = xSemaphoreCreateMutex();

    // Estimator gains

    SemaphoreGuard guard1(m_theta_mutex);
    SemaphoreGuard guard2(m_omega_mutex);

    if (guard1.acquired() && guard2.acquired())
    {

        // Set initial orientation angle
        m_theta = 0.0;
        // Set initial angular velocity
        m_omega = 0.0;
    }
}

bool Estimator::selectDevice()
{
    uint8_t status = m_devicesRef.getStatus();

    ESP_LOGI("ESTIMATOR", "Device status from estimator: %d", status);

    // prioritise IMU over ROT_ENC
    if ((status & IMU_BIT) == IMU_BIT)
    {
        m_imuSelected = true;
    }
    else if ((status & ROT_ENC_BIT) == ROT_ENC_BIT)

    {
        m_rotEncSelected = true;
    }
    else
    {
    }

    return (m_imuSelected || m_rotEncSelected);
}

// Angular velocity bias calibration for IMU
bool Estimator::calibrateOmegaBias()
{
    if (m_imuSelected)
    {
        // Calculate angular velocity bias by averaging n samples for 0.5 seconds
        int n_samples = static_cast<int>(500 / (aquisition_dt_ms));

        float omegaBias = 0.0;

        for (int i = 0; i < n_samples; i++)
        {
            m_devicesRef.m_imu.update();
            omegaBias += m_devicesRef.m_imu.getGyroY() / n_samples;
            vTaskDelay(pdMS_TO_TICKS(m_dt));
        }

        if (fabs(omegaBias) > 1.0) // !!! arbitrary threshold, but this value should be low
        {
            ESP_LOGE("ESTIMATOR", "Cube is moving too much for calibration, Omega = %f", omegaBias);
            return false;
        }

        m_omegaBias = omegaBias;
    }
    else
    {
        return false;
    }

    return true;
}

bool Estimator::calibrate()
{
    bool succ = false;

    succ = calibrateStartSide();  // set the position of the cube to either -90 or deg based on accel readings (where 0 is the upright position)
    succ &= calibrateOmegaBias(); // calibrate the angular velocity bias

    return succ;
}

bool Estimator::calibrateStartSide()
{
    if (m_imuSelected)
    {
        float ay = 0;
        float ax = 0;

        // averaging n samples for 0.25 seconds
        int n_samples = 1 / (4 * m_dt);

        for (int i = 0; i < n_samples; i++)
        {
            m_devicesRef.m_imu.update();
            ay += m_devicesRef.m_imu.getAccelY(); // If we read positive values for 'y', cube is upside down and we have to go to critical error state
            ax += m_devicesRef.m_imu.getAccelX(); // Sign only changes for 'x' when the cube is flipped
            vTaskDelay(pdMS_TO_TICKS(m_dt));
        }

        ay /= n_samples;
        ax /= n_samples;

        if (ay > 0)
        {
            ESP_LOGE("ESTIMATOR", "Cube is upside down!, [acel-y] reading: %f", ay);
            return false;
        }

        float gravity_45_threshold = GRAVITY_CONST * 0.707; // axes are at 45 degrees to sides.

        if (fabs(ax) < (gravity_45_threshold * 0.9)) // 0.9 is a safety factor
        {
            ESP_LOGE("ESTIMATOR", "Cube is not flat on its side!, [acel-x] reading: %f", ax);
            return false;
        }

        else if (ax < 0)
        {
            m_startAngle = -QUARTER_PI; // pivot point is at -45 (anti-clockwise from upright position)
        }
        else
        {
            m_startAngle = QUARTER_PI; // pivot point is at 45 (clockwise from upright position)
        }

        SemaphoreGuard guard(m_theta_mutex);
        if (guard.acquired())
        {
            m_theta = m_startAngle; // theta is always acculumated after this point
        }
        else
        {
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Estimator::estimateWithIMU()
{
    // Update to latest readings
    m_devicesRef.m_imu.update();

    // Get angular velocity from IMU gyroscope data
    float omega_measured = m_devicesRef.m_imu.getGyroY();

    // ESP_LOGI("ESTIMATOR", "Omega measured: %f", omega_measured);

    // Predict theta step
    predict(omega_measured);

    // Get linear acceleration from IMU accelerometer data
    float ax = m_devicesRef.m_imu.getAccelX(); // Assuming X-axis
    float ay = m_devicesRef.m_imu.getAccelY(); // Assuming Y-axis

    // Correct theta prediction using acceleration
    correct(ax, ay);

    // WrapTheta(); // wrap theta to [-π, π]
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
        estimateWithIMU();
    }

    if (m_rotEncSelected)
    {
        estimateWithRot_Enc(); // nothing here yet
    }
    // !!! need to somehow fuse these two, if they are both selected
}

void Estimator::estimateWithRot_Enc()
{
    // SemaphoreGuard guard(m_theta_mutex);
    // if (guard.acquired())
    // {
    //     // Example: Read encoder value and convert to angle
    //     float encoder_angle = m_devicesRef.m_rotEnc.getAngle();
    //     // Implement prediction and correction based on encoder data
    //     m_theta = encoder_angle; // multiply by time to ensure time scaling and consistency
    // }
}

// Predict step
void Estimator::predict(float omega_measured)
{
    SemaphoreGuard guard1(m_theta_mutex);
    SemaphoreGuard guard2(m_omega_mutex);
    if (guard1.acquired() && guard2.acquired())
    {
        // Correct the measured angular velocity with bias
        float omega_corrected = omega_measured - m_omegaBias;

        m_theta += omega_corrected * m_dt;
        m_omega = omega_corrected;
    }
}

// Correct step
void Estimator::correct(float ax, float ay)
{

    // Normalize linear acceleration
    float a_norm = sqrt(ax * ax + ay * ay);
    a_norm = (a_norm == 0) ? 1 : a_norm; // avoid division by zero
    ax /= a_norm;
    ay /= a_norm;

    SemaphoreGuard guard(m_theta_mutex);
    if (guard.acquired())
    {
        // should we account acceleration induced by reaction wheel?
        // Calculate the expected gravity direction
        float gravity_est_x = sin(m_theta);  // rotate by 90 degrees
        float gravity_est_y = -cos(m_theta); // expecting y to be negative

        // Compute error between measured and estimated gravity
        float error_x = ax - gravity_est_x;
        float error_y = ay - gravity_est_y;

        // Compute error angle (simplified)
        float error_theta = atan2(error_y, error_x);

        // Apply correction
        m_theta += m_lds * error_theta; // * m_dt; // multiply by time to ensure time scaling and consistency
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