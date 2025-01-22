#pragma once

/*
===============================
        User Defined
===============================
*/
#define ALLOW_SLEEP 1
#define SLEEP_TIMEOUT_MS 6000 // Time until sleep in idle state (ms)

// Device selection
#define USE_IMU 1
#define LOG_SD 1
#define LOG_SERIAL 0 // beware, this is slow
#define SILENT_INDICATION 0
#define SERVO_BRAKING 0
#define USE_ROT_ENC 0

#define DUMMY_IMU 0
#define DUMMY_SERVO 1
#define DUMMY_BLDC 1
#define DUMMY_MAG 1
#define DUMMY_ROT_ENC 1
#define DUMMY_SD 0
#define DUMMY_SERIAL 1
#define DUMMY_LED 0
// #define DUMMY_USBPD 1

// Task loop frequencies

#define AQUISITION_FREQ 250     // Hz
#define BALANCE_FREQ 100        // Hz
#define BLDC_FREQ 1000          // Hz
#define LOG_FREQ 50             // Hz
#define REFRESH_STATUS_FREQ 0.2 // Hz
#define INDICATION_FREQ 1       // Hz
#define TASK_MANAGER_FREQ 10    // Hz

// Log parameters
#define LOG_FILE_PREFIX "/LOG"

#define LOG_THETA 1
#define LOG_THETA_DOT 1
#define LOG_PHI 1
#define LOG_PHI_DOT 1
#define LOG_SETPOINT 0

#define LOG_COLUMNS (LOG_THETA + LOG_THETA_DOT + LOG_PHI + LOG_PHI_DOT)

// BLDC motor parameters
#define NUM_POLES 11          // pole pairs. 24N22P - how many pole pairs are there?
#define PHASE_RESISTANCE 11.1 // phase resistance
#define SENSE_MVPA 185.0      // ACS712-05B has the resolution of 0.185 (milli-Volts per Amp)

// Sensor parameters
// #define b_ax -0.0664 // Acelerometer bias and scale factor
// #define b_ay 0.1602
// #define f_ax 1.0065
// #define f_ay 1.0086

#define OMEGA_SET_Y_AXIS 1 // set to 1 if the omega is measured in the y-axis

// Control params
#define LQR 1
#define RATE_LIMIT 2.5 // Nm/s
#define MOTOR_KV 52.8  // Nm/A. This is also used to calc max torque (probably not accurate)

#if LQR
#define ANGLE_THRESH 0.5 // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg
// LQR Gain Matrix (precomputed offline)
#define LQR_K1 1.0
#define LQR_K2 1.0
#define LQR_K3 1.0
#define LQR_K4 1.0
#else
// #define balance_Ki 0.0 // using PD control for now
#define JERK_KP 1.0
#define JERK_KD 0.0

#define WHEEL_J 0.000928 // Moment of inertia of the wheel

#define ANGLE_THRESH 0.5   // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg
#define BALANCE_ANGLE 0.0  // radians. This is the angle at which the cube will balance. Needs to be calibrated to account for mass offsets
#define BALANCE_PERIOD 2.0 // seconds. This is the period of the balance trajectory
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