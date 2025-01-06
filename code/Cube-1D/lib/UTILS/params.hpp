#ifndef PARAMS_HPP
#define PARAMS_HPP

#define aquisitionFreq 10 // Hz
#define balanceFreq 5     // Hz
#define BLDCFreq 5        // Hz
#define logFreq 10        // Hz

#define refreshStatusFreq 0.2 // Hz
#define indicationFreq 1      // Hz

#define aquisition_dt_ms (1000.0 / aquisitionFreq)       // ms
#define balance_dt_ms (1000.0 / balanceFreq)             // ms
#define BLDC_dt_ms (1000.0 / BLDCFreq)                   // ms
#define log_dt_ms (1000.0 / logFreq)                     // ms
#define refreshStatus_dt_ms (1000.0 / refreshStatusFreq) // ms
#define indication_dt_ms (1000.0 / indicationFreq)       // ms

// Control and Estimator parameters

// Acelerometer bias and scale factor
#define b_ax -0.0664
#define b_ay 0.1602

#define f_ax 1.0065
#define f_ay 1.0086

#define AngleThresh 0.5 // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg

// BLDC motor parameters
#define num_poles 11   // pole pairs. 24N22P - how many pole pairs are there?
#define phase_res 11.1 // phase resistance
#define sense_mVpA 185.0     // ACS712-05B has the resolution of 0.185mV per Amp

#endif // PARAMS_HPP