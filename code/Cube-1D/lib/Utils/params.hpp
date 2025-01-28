#pragma once

/*
===============================
        User Defined
===============================
*/
#define ALLOW_SLEEP 1
#define SLEEP_TIMEOUT_MS 20000 // Time until sleep in idle state (ms)

// Device selection
#define LOG_SD 0
#define LOG_SERIAL 1 // beware, this is slow
#define SILENT_INDICATION 0
#define SERVO_BRAKING 0
#define USE_ROT_ENC 0
#define USE_IMU 1

#define DUMMY_IMU 0
#define DUMMY_SERVO 1
#define DUMMY_BLDC 0
#define DUMMY_MAG 0
#define DUMMY_ROT_ENC 1
#define DUMMY_SD 0
#define DUMMY_USBPD 1

// Task loop frequencies
#define AQUISITION_FREQ 400f     // Hz
#define BALANCE_FREQ 100f        // Hz
#define BLDC_FREQ 1000f          // Hz
#define LOG_FREQ 100f            // Hz
#define REFRESH_STATUS_FREQ 0.2f // Hz
#define INDICATION_FREQ 1f       // Hz
#define TASK_MANAGER_FREQ 10f    // Hz

// Log parameters
#define LOG_FILE_PREFIX "/LOG"

#define LOG_THETA 1
#define LOG_THETA_DOT 1
#define LOG_PHI 1
#define LOG_PHI_DOT 1
#define LOG_SETPOINT 1

#define LOG_COLUMNS (LOG_THETA + LOG_THETA_DOT + LOG_PHI + LOG_PHI_DOT + LOG_SETPOINT)

// BLDC motor parameters
#define NUM_POLES 11           // pole pairs. 24N22P - how many pole pairs are there?
#define PHASE_RESISTANCE 11.1f // phase resistance
#define SENSE_MVPA 185.0f      // ACS712-05B has the resolution of 0.185 (milli-Volts per Amp)

// Sensor parameters
// #define b_ax -0.0664 // Acelerometer bias and scale factor
// #define b_ay 0.1602
// #define f_ax 1.0065
// #define f_ay 1.0086

#define OMEGA_SET_Z_AXIS 1 // set to 1 if the omega is measured in the y-axis

// Control params
#define LQR 1
#define RATE_LIMIT 100.0f // Nm/s
#define MOTOR_KV 52.8f    // Nm/A. This is also used to calc max torque (probably not accurate)

#if LQR
#define ANGLE_THRESH 0.4f // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg
// LQR Gain Matrix (precomputed offline)
#define LQR_K1 -1.0f   // theta
#define LQR_K2 -0.3f   // theta_dot
#define LQR_K3 0.0f    // phi
#define LQR_K4 -0.002f // phi_dot
#else
// #define balance_Ki 0.0 // using PD control for now
#define JERK_KP 1.0f
#define JERK_KD 0.0f

#define WHEEL_J 0.000928f // Moment of inertia of the wheel

#define ANGLE_THRESH 0.5f   // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg
#define BALANCE_ANGLE 0.0f  // radians. This is the angle at which the cube will balance. Needs to be calibrated to account for mass offsets
#define BALANCE_PERIOD 2.0f // seconds. This is the period of the balance trajectory
#endif

#if USE_ROT_ENC
#define ENC_PPR 600
#endif

/*
===============================
        Auto Defined
===============================
*/
// Task loop periods
#define AQUISITION_MS (1000.0 / AQUISITION_FREQ)         // ms
#define BALANCE_MS (1000.0 / BALANCE_FREQ)               // ms
#define BLDC_MS (1000.0 / BLDC_FREQ)                     // ms
#define LOG_MS (1000.0 / LOG_FREQ)                       // ms
#define REFRESH_STATUS_MS (1000.0 / REFRESH_STATUS_FREQ) // ms
#define INDICATION_MS (1000.0 / INDICATION_FREQ)         // ms
#define TASK_MANAGER_MS (1000.0 / TASK_MANAGER_FREQ)     // ms