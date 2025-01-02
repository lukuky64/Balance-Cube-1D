#ifndef estimator_attitude_h
#define estimator_attitude_h

#include "Arduino.h"
#include "esp_log.h"

// Attitude estimator class
class Estimator
{
public:
    // Constructor
    Estimator();
    // Initializer
    void init();
    // Estimate step
    void estimate();
    // Rotation quaternion estimations
    float q0, q1, q2, q3;
    // Angular velocity (rad/s) estimations
    float omega_x, omega_y, omega_z;

private:
    // Angular velocity bias calibration
    void calibrate();
    // Predict step
    void predict(float omega_x, float omega_y, float omega_z);
    // Correct step
    void correct(float ax, float ay, float az);
    // Angular velocity (rad/s) bias
    float b_omega_x, b_omega_y, b_omega_z;

    // Estimator gains
    float lds;
    float ldw;
};

#endif