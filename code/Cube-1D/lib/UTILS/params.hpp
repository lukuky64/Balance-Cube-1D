#ifndef PARAMS_HPP
#define PARAMS_HPP

#define aquisitionFreq 200 // Hz
#define balanceFreq 100    // Hz
#define BLDCFreq 100       // Hz
#define logFreq 20         // Hz

#define aquisition_dt_ms (1000.0 / aquisitionFreq) // ms
#define balance_dt_ms (1000.0 / balanceFreq)       // ms
#define BLDC_dt_ms (1000.0 / BLDCFreq)             // ms
#define log_dt_ms (1000.0 / logFreq)               // ms

// Control and Estimator parameters

// Acelerometer bias and scale factor
#define b_ax -0.0664
#define b_ay 0.1602

#define f_ax 1.0065
#define f_ay 1.0086

#define AngleThresh 0.5 // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg

#endif // PARAMS_HPP