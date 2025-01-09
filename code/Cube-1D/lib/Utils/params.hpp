#pragma once

/*
===============================
        User Defined
===============================
*/

// Device selection
#define USE_IMU 1
#define LOG_SD 1
#define LOG_SERIAL 0 // beware, this is slow
#define SILENT_INDICATION 1
#define SERVO_BRAKING 0
#define USE_ROT_ENC 0

#define DUMMY_IMU 1
#define DUMMY_SERVO 1
#define DUMMY_BLDC 1
#define DUMMY_MAG 1
#define DUMMY_ROT_ENC 1
#define DUMMY_SD 0
#define DUMMY_SERIAL 1
#define DUMMY_LED 0
#define DUMMY_USBPD 1

// Task loop frequencies
#define aquisitionFreq 100    // Hz
#define balanceFreq 25        // Hz
#define BLDCFreq 250          // Hz
#define logFreq 25            // Hz
#define refreshStatusFreq 0.2 // Hz
#define indicationFreq 1      // Hz

// Log parameters
#define log_header "Time(s),Theta(rad),Omega(rad/s),Mag(rad)"
#define log_file_prefix "/LOG"
#define log_columns 3

// BLDC motor parameters
#define num_poles 11     // pole pairs. 24N22P - how many pole pairs are there?
#define phase_res 11.1   // phase resistance
#define sense_mVpA 185.0 // ACS712-05B has the resolution of 0.185mV per Amp

// Control and Estimator parameters
#define b_ax -0.0664 // Acelerometer bias and scale factor
#define b_ay 0.1602
#define f_ax 1.0065
#define f_ay 1.0086
#define AngleThresh 0.5 // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg

#define wheel_J 0.0001 // kg*m^2. Moment of inertia of the wheel

#define balanceAngle 0.0  // radians. This is the angle at which the cube will balance. Needs to be calibrated to account for mass offsets
#define balancePeriod 2.0 // seconds. This is the period of the balance trajectory

#define balance_Kp 1.0
#define balance_Kd 0.0
// #define balance_Ki 0.0 // using PD control for now

#define ENC_PPR 600

/*
===============================
        Auto Defined
===============================
*/
// Task loop periods
#define aquisition_dt_ms (1000.0 / aquisitionFreq)       // ms
#define balance_dt_ms (1000.0 / balanceFreq)             // ms
#define BLDC_dt_ms (1000.0 / BLDCFreq)                   // ms
#define log_dt_ms (1000.0 / logFreq)                     // ms
#define refreshStatus_dt_ms (1000.0 / refreshStatusFreq) // ms
#define indication_dt_ms (1000.0 / indicationFreq)       // ms