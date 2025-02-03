#pragma once

#include <Arduino.h>

#define LQR 1
#define MAX_LOG_COLUMNS 10

// Dummy device selection
#define DUMMY_IMU 0
#define DUMMY_SERVO 1
#define DUMMY_BLDC 0
#define DUMMY_MAG 0
#define DUMMY_ROT_ENC 1
#define DUMMY_SD 0
#define DUMMY_USBPD 1

#define OMEGA_SET_Z_AXIS 1

#define USE_WIFI 1 // when enabled, tasks will be pinned to specific cores

namespace Params
{
        // Global parameter declarations (extern means defined elsewhere)
        extern unsigned int ALLOW_SLEEP;
        extern float SLP_TIMEOUT_MS;
        extern float AQUISITION_FREQ;
        extern float BALANCE_FREQ;
        extern float BLDC_FREQ;
        extern float LOG_FREQ;
        extern float REFR_STAT_FREQ;
        extern float INDICATION_FREQ;
        extern float TASK_MANGR_FREQ;

        extern unsigned int LOG_SD;
        extern unsigned int LOG_SERIAL;
        extern unsigned int SILENT_INDICATION;
        extern unsigned int SERVO_BRAKING;
        extern unsigned int USE_ROT_ENC;
        extern unsigned int USE_IMU;

        extern const char *LOG_FILE_PREFIX;
        extern unsigned int LOG_THETA;
        extern unsigned int LOG_THETA_DOT;
        extern unsigned int LOG_PHI;
        extern unsigned int LOG_PHI_DOT;
        extern unsigned int LOG_SETPOINT;

        extern unsigned int NUM_POLES;
        extern float PHASE_RES;
        extern float SENSE_MVPA;

        extern float RATE_LIMIT;
        extern float MOTOR_KV;
        extern float ANGLE_THRESH;
        extern float LQR_K1;
        extern float LQR_K2;
        extern float LQR_K3;
        extern float LQR_K4;

        extern float JERK_KP;
        extern float JERK_KD;
        extern float WHEEL_J;
        extern float BALANCE_ANGLE;
        extern float BALANCE_PERIOD;

        extern unsigned int ENC_PPR;
        extern unsigned int LOG_COLUMNS;

        // Periods (in ms)
        extern float AQUISITION_MS;
        extern float BALANCE_MS;
        extern float BLDC_MS;
        extern float LOG_MS;
        extern float REFRESH_STATUS_MS;
        extern float INDICATION_MS;
        extern float TASK_MANAGER_MS;

        // Function declarations
        void recalculatePeriods();
        void loadPreferences();
        void savePreference(const char *key, float value);
        void setFrequency(const char *keyFreq, float &freq, float newFreq);
        void wipeSettings();
        float getFloat(const char *key);
        unsigned int getUInt(const char *key);
        bool exists(const char *key);
}
