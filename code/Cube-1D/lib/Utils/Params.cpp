#include "Params.hpp"
#include <Preferences.h>
#include "esp_log.h"
#include <cstring>

namespace Params
{
        // Global parameter definitions
        unsigned int ALLOW_SLEEP = 1;
        float SLP_TIMEOUT_MS = 30000.0f;
        float AQUISITION_FREQ = 400.0f;
        float BALANCE_FREQ = 200.0f;
        float BLDC_FREQ = 1000.0f;
        float LOG_FREQ = 100.0f;
        float REFR_STAT_FREQ = 0.2f;
        float INDICATION_FREQ = 1.0f;
        float TASK_MANGR_FREQ = 10.0f;

        unsigned int LOG_SD = 1;
        unsigned int LOG_SERIAL = 0;
        unsigned int SILENT_INDICATION = 0;
        unsigned int SERVO_BRAKING = 0;
        unsigned int USE_ROT_ENC = 0;
        unsigned int USE_IMU = 1;

        const char *LOG_FILE_PREFIX = "/LOG";
        unsigned int LOG_THETA = 1;
        unsigned int LOG_THETA_DOT = 1;
        unsigned int LOG_PHI = 1;
        unsigned int LOG_PHI_DOT = 1;
        unsigned int LOG_SETPOINT = 1;

        unsigned int NUM_POLES = 11;
        float PHASE_RES = 11.1f;
        float SENSE_MVPA = 185.0f;

        float RATE_LIMIT = 100.0f;
        float MOTOR_KV = 52.8f;
        float ANGLE_THRESH = 0.4f;
        float LQR_K1 = -1.35f;
        float LQR_K2 = -0.3f;
        float LQR_K3 = -0.0f;
        float LQR_K4 = -0.002f;

        float JERK_KP = 1.0f;
        float JERK_KD = 0.0f;
        float WHEEL_J = 0.000928f;
        float BALANCE_ANGLE = 0.0f;
        float BALANCE_PERIOD = 2.0f;

        unsigned int ENC_PPR = 600;
        unsigned int LOG_COLUMNS = (LOG_THETA + LOG_THETA_DOT + LOG_PHI + LOG_PHI_DOT + LOG_SETPOINT);

        // Calculate initial periods (in ms)
        float AQUISITION_MS = (1000.0f / AQUISITION_FREQ);
        float BALANCE_MS = (1000.0f / BALANCE_FREQ);
        float BLDC_MS = (1000.0f / BLDC_FREQ);
        float LOG_MS = (1000.0f / LOG_FREQ);
        float REFRESH_STATUS_MS = (1000.0f / REFR_STAT_FREQ);
        float INDICATION_MS = (1000.0f / INDICATION_FREQ);
        float TASK_MANAGER_MS = (1000.0f / TASK_MANGR_FREQ);

        void recalculatePeriods()
        {
                AQUISITION_MS = (1000.0f / AQUISITION_FREQ);
                BALANCE_MS = (1000.0f / BALANCE_FREQ);
                BLDC_MS = (1000.0f / BLDC_FREQ);
                LOG_MS = (1000.0f / LOG_FREQ);
                REFRESH_STATUS_MS = (1000.0f / REFR_STAT_FREQ);
                INDICATION_MS = (1000.0f / INDICATION_FREQ);
                TASK_MANAGER_MS = (1000.0f / TASK_MANGR_FREQ);
        }

        // Example implementations:
        void loadPreferences()
        {
                Preferences preferences;
                if (preferences.begin("params", false))
                { // Open in RW mode
#define LOAD_OR_INIT(pref, type, var, defaultVal)                                     \
        if (pref.isKey(#var))                                                         \
        {                                                                             \
                var = pref.get##type(#var, defaultVal);                               \
                ESP_LOGI("Params", "Loaded %s = %f", #var, (float)var);               \
        }                                                                             \
        else                                                                          \
        {                                                                             \
                ESP_LOGW("Params", "Key %s not found. Initialising default: %s = %f", \
                         #var, #var, (float)defaultVal);                              \
                pref.put##type(#var, defaultVal);                                     \
                var = defaultVal;                                                     \
        }

                        // Load or initialize values
                        LOAD_OR_INIT(preferences, UInt, ALLOW_SLEEP, 1);
                        LOAD_OR_INIT(preferences, Float, SLP_TIMEOUT_MS, 30000.0f);
                        LOAD_OR_INIT(preferences, Float, AQUISITION_FREQ, 400.0f);
                        LOAD_OR_INIT(preferences, Float, BALANCE_FREQ, 200.0f);
                        LOAD_OR_INIT(preferences, Float, BLDC_FREQ, 1000.0f);
                        LOAD_OR_INIT(preferences, Float, LOG_FREQ, 100.0f);
                        LOAD_OR_INIT(preferences, Float, REFR_STAT_FREQ, 0.2f);
                        LOAD_OR_INIT(preferences, Float, INDICATION_FREQ, 1.0f);
                        LOAD_OR_INIT(preferences, Float, TASK_MANGR_FREQ, 10.0f);
                        LOAD_OR_INIT(preferences, Float, PHASE_RES, 11.1f);
                        LOAD_OR_INIT(preferences, Float, SENSE_MVPA, 185.0f);
                        LOAD_OR_INIT(preferences, Float, RATE_LIMIT, 100.0f);
                        LOAD_OR_INIT(preferences, Float, MOTOR_KV, 52.8f);
                        LOAD_OR_INIT(preferences, Float, LQR_K1, -1.2f);
                        LOAD_OR_INIT(preferences, Float, LQR_K2, -0.25f);
                        LOAD_OR_INIT(preferences, Float, LQR_K3, -0.0f);
                        LOAD_OR_INIT(preferences, Float, LQR_K4, -0.002f);
                        LOAD_OR_INIT(preferences, Float, JERK_KP, 1.0f);
                        LOAD_OR_INIT(preferences, Float, JERK_KD, 0.0f);
                        LOAD_OR_INIT(preferences, Float, WHEEL_J, 0.000928f);
                        LOAD_OR_INIT(preferences, Float, ANGLE_THRESH, 0.4f);
                        LOAD_OR_INIT(preferences, Float, BALANCE_ANGLE, 0.0f);
                        LOAD_OR_INIT(preferences, Float, BALANCE_PERIOD, 2.0f);

                        recalculatePeriods();
                        preferences.end();
                        ESP_LOGI("Params", "Preferences loaded.");
                }
                else
                {
                        ESP_LOGE("Params", "Failed to open NVS storage!");
                }
        }

        void savePreference(const char *key, float value)
        {
                Preferences preferences;
                if (preferences.begin("params", false))
                {
                        preferences.putFloat(key, value);
                        ESP_LOGI("Params", "Stored float preference: %s = %f", key, value);

                        // Update in-RAM variable if applicable (for example, for LQR_K1)
                        if (strcmp(key, "LQR_K1") == 0)
                        {
                                LQR_K1 = value;
                                ESP_LOGI("Params", "Updated LQR_K1 in RAM: %f", LQR_K1);
                        }
                        preferences.end();
                }
                else
                {
                        ESP_LOGE("Params", "Failed to open NVS storage!");
                }
        }

        void setFrequency(const char *keyFreq, float &freq, float newFreq)
        {
                freq = newFreq;
                savePreference(keyFreq, freq);
                recalculatePeriods();
        }

        void wipeSettings()
        {
                ESP_LOGW("NVS", "Wiping preferences and rebooting...");
                Preferences preferences;
                if (preferences.begin("params", false))
                {
                        preferences.clear();
                        preferences.end();
                        ESP_LOGW("NVS", "Preferences wiped!");
                }
                else
                {
                        ESP_LOGE("NVS", "Failed to open NVS storage!");
                }
                delay(1000);
                esp_restart();
        }

        float getFloat(const char *key)
        {
                Preferences preferences;
                float val = 0.0f;
                if (preferences.begin("params", false))
                {
                        val = preferences.getFloat(key, 0.0f);
                        preferences.end();
                }
                return val;
        }

        unsigned int getUInt(const char *key)
        {
                Preferences preferences;
                unsigned int val = 0;
                if (preferences.begin("params", false))
                {
                        val = preferences.getUInt(key, 0);
                        preferences.end();
                }
                return val;
        }

        bool exists(const char *key)
        {
                Preferences preferences;
                bool keyExists = false;
                if (preferences.begin("params", false))
                {
                        keyExists = preferences.isKey(key);
                        preferences.end();
                }
                return keyExists;
        }
}
