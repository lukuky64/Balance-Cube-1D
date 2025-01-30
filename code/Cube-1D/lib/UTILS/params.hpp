#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "esp_log.h"

#define LOAD_PARAM(pref, type, var, defaultVal) var = pref.get##type(#var, defaultVal);

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

// Namespace to encapsulate all variables and functions
namespace Params
{
        static unsigned int ALLOW_SLEEP = 1;
        static unsigned int SLP_TIMEOUT_MS = 30000; // Time until sleep in idle state (ms)

        // Task loop frequencies
        static float AQUISITION_FREQ = 400.0f; // Hz
        static float BALANCE_FREQ = 100.0f;    // Hz
        static float BLDC_FREQ = 1000.0f;      // Hz
        static float LOG_FREQ = 100.0f;        // Hz
        static float REFR_STAT_FREQ = 0.2f;    // Hz
        static float INDICATION_FREQ = 1.0f;   // Hz
        static float TASK_MANGR_FREQ = 10.0f;  // Hz

        // Device preference selection
        static unsigned int LOG_SD = 1;
        static unsigned int LOG_SERIAL = 0; // beware, this is slow
        static unsigned int SILENT_INDICATION = 0;
        static unsigned int SERVO_BRAKING = 0;
        static unsigned int USE_ROT_ENC = 0;
        static unsigned int USE_IMU = 1;

        // Log parameters
        static const char *LOG_FILE_PREFIX = "/LOG";
        static unsigned int LOG_THETA = 1;
        static unsigned int LOG_THETA_DOT = 1;
        static unsigned int LOG_PHI = 1;
        static unsigned int LOG_PHI_DOT = 1;
        static unsigned int LOG_SETPOINT = 1;

        // BLDC motor parameters
        static unsigned int NUM_POLES = 11; // pole pairs. 24N22P - how many pole pairs are there?
        static float PHASE_RES = 11.1f;     // phase resistance
        static float SENSE_MVPA = 185.0f;   // ACS712-05B has the resolution of 0.185 (milli-Volts per Amp)

        // Control params
        static float RATE_LIMIT = 100.0f; // Nm/s. Large value so we can ignore for now
        static float MOTOR_KV = 52.8f;    // Nm/A. This is also used to calc max torque (probably not accurate)

        static float ANGLE_THRESH = 0.4f; // Threshold for controllable bounds. Radians, 0.5 rad = 28.6 deg
        // LQR Gain Matrix (precomputed offline)
        static float LQR_K1 = -1.4f;    // theta
        static float LQR_K2 = -0.3f;    // theta_dot
        static float LQR_K3 = -0.0f;    // phi. If we want to use this, best to wrap phi to 0 to 2pi in the LQR class, ortherwise its hard to recover from multiple rotations
        static float LQR_K4 = -0.0023f; // phi_dot

        // Should just get rid of these since we aren't using the min jerk controller
        static float JERK_KP = 1.0f;
        static float JERK_KD = 0.0f;
        static float WHEEL_J = 0.000928f;   // Moment of inertia of the wheel
        static float BALANCE_ANGLE = 0.0f;  // radians. This is the angle at which the cube will balance. Needs to be calibrated to account for mass offsets
        static float BALANCE_PERIOD = 2.0f; // seconds. This is the period of the balance trajectory

        static unsigned int ENC_PPR = 600;

        static unsigned int LOG_COLUMNS = (LOG_THETA + LOG_THETA_DOT + LOG_PHI + LOG_PHI_DOT + LOG_SETPOINT); // Number of columns in the log file

        // Periods (calculated from frequencies)
        static float AQUISITION_MS = (1000.0 / AQUISITION_FREQ);    // ms
        static float BALANCE_MS = (1000.0 / BALANCE_FREQ);          // ms
        static float BLDC_MS = (1000.0 / BLDC_FREQ);                // ms
        static float LOG_MS = (1000.0 / LOG_FREQ);                  // ms
        static float REFRESH_STATUS_MS = (1000.0 / REFR_STAT_FREQ); // ms
        static float INDICATION_MS = (1000.0 / INDICATION_FREQ);    // ms
        static float TASK_MANAGER_MS = (1000.0 / TASK_MANGR_FREQ);  // ms

        // Recalculate dependent periods
        static void recalculatePeriods()
        {
                AQUISITION_MS = (1000.0 / AQUISITION_FREQ);
                BALANCE_MS = (1000.0 / BALANCE_FREQ);
                BLDC_MS = (1000.0 / BLDC_FREQ);
                LOG_MS = (1000.0 / LOG_FREQ);
                REFRESH_STATUS_MS = (1000.0 / REFR_STAT_FREQ);
                INDICATION_MS = (1000.0 / INDICATION_FREQ);
                TASK_MANAGER_MS = (1000.0 / TASK_MANGR_FREQ);
        }

        // Functions to handle persistent storage
        static void loadPreferences()
        {
                Preferences preferences;
                if (preferences.begin("params", false)) // Open in RW mode
                {
// Helper macro to check and initialize parameters
#define LOAD_OR_INIT(pref, type, var, defaultVal)                                                                     \
        if (!pref.isKey(#var))                                                                                        \
        {                                                                                                             \
                ESP_LOGW("Params", "Key %s not found. Initializing default: %s = %f", #var, #var, (float)defaultVal); \
                pref.put##type(#var, defaultVal);                                                                     \
                var = defaultVal;                                                                                     \
        }                                                                                                             \
        else                                                                                                          \
        {                                                                                                             \
                var = pref.get##type(#var, defaultVal);                                                               \
        }
                        // Load or initialize values
                        LOAD_OR_INIT(preferences, UInt, ALLOW_SLEEP, 1);
                        LOAD_OR_INIT(preferences, UInt, SLP_TIMEOUT_MS, 30000);
                        LOAD_OR_INIT(preferences, Float, AQUISITION_FREQ, 400.0f);
                        LOAD_OR_INIT(preferences, Float, BALANCE_FREQ, 100.0f);
                        LOAD_OR_INIT(preferences, Float, BLDC_FREQ, 1000.0f);
                        LOAD_OR_INIT(preferences, Float, LOG_FREQ, 100.0f);
                        LOAD_OR_INIT(preferences, Float, REFR_STAT_FREQ, 0.2f);
                        LOAD_OR_INIT(preferences, Float, INDICATION_FREQ, 1.0f);
                        LOAD_OR_INIT(preferences, Float, TASK_MANGR_FREQ, 10.0f);
                        LOAD_OR_INIT(preferences, Float, PHASE_RES, 11.1f);
                        LOAD_OR_INIT(preferences, Float, SENSE_MVPA, 185.0f);
                        LOAD_OR_INIT(preferences, Float, RATE_LIMIT, 100.0f);
                        LOAD_OR_INIT(preferences, Float, MOTOR_KV, 52.8f);
                        LOAD_OR_INIT(preferences, Float, LQR_K1, -1.4f);
                        LOAD_OR_INIT(preferences, Float, LQR_K2, -0.3f);
                        LOAD_OR_INIT(preferences, Float, LQR_K3, -0.0f);
                        LOAD_OR_INIT(preferences, Float, LQR_K4, -0.0023f);
                        LOAD_OR_INIT(preferences, Float, JERK_KP, 1.0f);
                        LOAD_OR_INIT(preferences, Float, JERK_KD, 0.0f);
                        LOAD_OR_INIT(preferences, Float, WHEEL_J, 0.000928f);
                        LOAD_OR_INIT(preferences, Float, ANGLE_THRESH, 0.4f);
                        LOAD_OR_INIT(preferences, Float, BALANCE_ANGLE, 0.0f);
                        LOAD_OR_INIT(preferences, Float, BALANCE_PERIOD, 2.0f);

                        recalculatePeriods(); // Update dependent calculations
                        preferences.end();

                        ESP_LOGI("Params", "Preferences loaded.");
                }
                else
                {
                        ESP_LOGE("Params", "Failed to open NVS storage!");
                }
        }

        // Save preferences to NVS
        template <typename T>
        static void savePreference(const char *key, T value)
        {
                Preferences preferences;
                if (preferences.begin("params", false)) // Open in RW mode
                {

                        if constexpr (std::is_same<T, unsigned int>::value)
                        {
                                preferences.putUInt(key, value);
                        }
                        else if constexpr (std::is_same<T, float>::value)
                        {
                                preferences.putFloat(key, value);
                        }
                        else
                        {
                                ESP_LOGE("Params", "Unsupported type for saving preference.");
                        }

                        preferences.end();
                }
                else
                {
                        ESP_LOGE("Params", "Failed to open NVS storage!");
                }
        }

        // Set frequency and recalculate dependent periods
        static void setFrequency(const char *keyFreq, float &freq, float newFreq)
        {
                freq = newFreq;

                // Save the frequency to NVS
                savePreference(keyFreq, freq);

                // Recalculate dependent variables
                recalculatePeriods();
        }

        static void wipeSettings()
        {
                ESP_LOGW("NVS", "Wiping preferences and rebooting...");

                Preferences preferences;
                if (preferences.begin("params", false)) // Open in RW mode
                {
                        preferences.clear(); // Erase all stored values
                        preferences.end();
                        ESP_LOGW("NVS", "Preferences wiped!");
                }
                else
                {
                        ESP_LOGE("NVS", "Failed to open NVS storage!");
                }

                delay(1000);
                esp_restart(); // Restart ESP32 to apply changes
        }
}
