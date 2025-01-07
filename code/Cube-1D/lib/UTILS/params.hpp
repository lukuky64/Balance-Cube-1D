#ifndef PARAMS_HPP
#define PARAMS_HPP

<<<<<<< HEAD
// Task loop frequencies
#define aquisitionFreq 10     // Hz
#define balanceFreq 5         // Hz
#define BLDCFreq 5            // Hz
#define logFreq 100           // Hz
=======
#define aquisitionFreq 10 // Hz
#define balanceFreq 5     // Hz
#define BLDCFreq 5        // Hz
#define logFreq 100       // Hz

>>>>>>> db244d9f2ec284d269a99dfba5b49c0b5842e2f1
#define refreshStatusFreq 0.2 // Hz
#define indicationFreq 1      // Hz

// Task loop periods
#define aquisition_dt_ms (1000.0 / aquisitionFreq)       // ms
#define balance_dt_ms (1000.0 / balanceFreq)             // ms
#define BLDC_dt_ms (1000.0 / BLDCFreq)                   // ms
#define log_dt_ms (1000.0 / logFreq)                     // ms
#define refreshStatus_dt_ms (1000.0 / refreshStatusFreq) // ms
#define indication_dt_ms (1000.0 / indicationFreq)       // ms

// Log parameters
#define log_header "Time(s),Theta(rad),Omega(rad/s),Mag(rad)"
#define log_file_prefix "/LOG"
#define log_columns 3

// BLDC motor parameters
#define num_poles 11     // pole pairs. 24N22P - how many pole pairs are there?
#define phase_res 11.1   // phase resistance
#define sense_mVpA 185.0 // ACS712-05B has the resolution of 0.185mV per Amp
<<<<<<< HEAD

// Control and Estimator parameters, Acelerometer bias and scale factor
#define b_ax -0.0664
#define b_ay 0.1602
#define f_ax 1.0065
#define f_ay 1.0086
#define AngleThresh 0.5 // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg
=======
>>>>>>> db244d9f2ec284d269a99dfba5b49c0b5842e2f1

#endif // PARAMS_HPP