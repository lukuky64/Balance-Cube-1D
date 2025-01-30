#include "INDICATORS.hpp"

Indicators::Indicators()
{
    statusMutex = xSemaphoreCreateMutex(); // Create the status mutex
}

Indicators::~Indicators()
{
    // Disable the buzzer and RGB LED

    m_buzzerEnabled = false;
    m_RGBLedEnabled = false;

    // Delete the mutexes
    vSemaphoreDelete(buzzerMutex);
    vSemaphoreDelete(rgbLedMutex);
    vSemaphoreDelete(statusMutex);
}

/*****************************************************************************/
/*!
    @brief  Setup the buzzer and create the buzzer mutex. Play a quick chirp
    to indicate success.

    @param  BuzzerPin The GPIO pin to be used for the buzzer
*/
/*****************************************************************************/
void Indicators::setupBuzzer(uint8_t buzzerPin)
{
    m_buzzerPin = buzzerPin;
    m_buzzerEnabled = true;
    pinMode(m_buzzerPin, OUTPUT);

    buzzerMutex = xSemaphoreCreateMutex(); // Create buzzer mutex

    // quick chirp
    controlBuzzer(tones[6].frequency, 50);
}

/*****************************************************************************/
/*!
    @brief Setup the RGB LED and create the RGB LED mutex. Flash the LED
    to indicate success.

    @param ledNeo The GPIO pin to be used for the RGB LED. Assuming the
    LED is an addressable LED (e.g. WS2812B).
*/
/*****************************************************************************/
void Indicators::setupRGBLed(uint8_t ledNeo)
{
    m_ledNeo = ledNeo;
    m_RGBLedEnabled = true;
    pinMode(m_ledNeo, OUTPUT);

    rgbLedMutex = xSemaphoreCreateMutex(); // Create the RGB LED mutex

    // quick test
    controlRGBLed(colours[1].value, 50); // Green
}

/*****************************************************************************/
/*!
    @brief Show a critical error indication. This will play a tone and flash red.
*/
/*****************************************************************************/
void Indicators::showCriticalError()
{
    ESP_LOGI(TAG, "Showing Critical Error!");

    uint16_t duration = 50; // ms

    for (int i = 0; i < 3; i++)
    {
        controlBuzzer(tones[0].frequency, duration);
        controlRGBLed(colours[0].value, duration); // Red
        vTaskDelay(pdMS_TO_TICKS(duration));       // off period
    }
}

/*****************************************************************************/
/*!
    @brief Show a warning indication. This will flash orange two times.
*/
/*****************************************************************************/
void Indicators::showWarning()
{
    ESP_LOGI(TAG, "Showing Warning!");

    uint16_t duration = 50; // ms

    for (int i = 0; i < 2; i++)
    {
        // controlBuzzer(tones[3].frequency, duration);
        controlRGBLed(colours[7].value, duration); // Orange
        vTaskDelay(pdMS_TO_TICKS(duration));       // off period
    }
}

/*****************************************************************************/
/*!
    @brief Show a success indication. This will play a tone and flash green
    three times.
*/
/*****************************************************************************/
void Indicators::showSuccess()
{
    ESP_LOGI(TAG, "Showing Success!");

    uint16_t duration = 50; // ms

    for (int i = 0; i < 3; i++)
    {
        controlBuzzer(tones[i].frequency, duration);
        controlRGBLed(colours[1].value, duration); // Green
        vTaskDelay(pdMS_TO_TICKS(duration));
    }
}

/*****************************************************************************/
/*!
    @brief Show an all good indication. This will flash green once.
*/
/*****************************************************************************/
void Indicators::showAllGood()
{
    // ESP_LOGI(TAG, "Showing All Good!");

    uint16_t duration = 50; // ms

    controlRGBLed(colours[1].value, duration);
}

/*****************************************************************************/
/*!
    @brief Turn off all indicators. This will turn off the buzzer and RGB LED.
*/
/*****************************************************************************/
void Indicators::showAllOff()
{
    ESP_LOGI(TAG, "Turning off all Indicators!");

    controlRGBLed(0, 0);
    controlBuzzer(300, 50);
}

/*****************************************************************************/
/*!
    @brief Control the buzzer. This will play a tone at a given frequency
    for a given duration.

    @param frequency The frequency of the tone to be played (Hz)

    @param duration The duration of the tone to be played (ms)

    @return True if the buzzer was setup successfully, false otherwise.
*/
/*****************************************************************************/
bool Indicators::controlBuzzer(int frequency, int duration)
{
    if (m_buzzerEnabled)
    {
        SemaphoreGuard guard(buzzerMutex);
        if (guard.acquired())
        {
            tone(m_buzzerPin, frequency);
            vTaskDelay(pdMS_TO_TICKS(duration));
            noTone(m_buzzerPin);
            return true;
        }
    }
    return false;
}

/*****************************************************************************/
/*!
    @brief Control the RGB LED. This will flash the LED a given colour for
    a given duration.

    @param hexValue The hex value of the colour to be displayed (0xRRGGBB)

    @param duration The duration of the colour to be displayed (ms)

    @return True if the RGB LED was setup successfully, false otherwise.
*/
/*****************************************************************************/
bool Indicators::controlRGBLed(int hexValue, int duration)
{
    if (m_RGBLedEnabled)
    {

        SemaphoreGuard guard(rgbLedMutex);
        if (guard.acquired())
        {
            uint8_t red = (hexValue >> 16) & 0xFF;
            uint8_t green = (hexValue >> 8) & 0xFF;
            uint8_t blue = hexValue & 0xFF;

            neopixelWrite(m_ledNeo, red, green, blue); // Red

            vTaskDelay(pdMS_TO_TICKS(duration));

            neopixelWrite(m_ledNeo, 0, 0, 0); // Red

            return true;
        }
    }
    return false;
}

/*****************************************************************************/
/*!
    @brief Check the status of the indicators.

    @return True if the buzzer or RGB LED is enabled, false otherwise.
*/
/*****************************************************************************/
bool Indicators::checkStatus()
{
    SemaphoreGuard guard(statusMutex);
    if (guard.acquired())
    {
        return m_buzzerEnabled || m_RGBLedEnabled;
    }
    return false;
}

/*****************************************************************************/
/*!
    @brief Check the status of the buzzer.

    @return True if the buzzer is enabled, false otherwise.
*/
/*****************************************************************************/
bool Indicators::checkStatusBuzzer()
{
    SemaphoreGuard guard(statusMutex);
    if (guard.acquired())
    {
        return m_buzzerEnabled;
    }
    return false;
}

/*****************************************************************************/
/*!
    @brief Check the status of the RGB LED.

    @return True if the RGB LED is enabled, false otherwise.
*/
/*****************************************************************************/
bool Indicators::checkStatusLed()
{
    SemaphoreGuard guard(statusMutex);
    if (guard.acquired())
    {
        return m_RGBLedEnabled;
    }
    return false;
}