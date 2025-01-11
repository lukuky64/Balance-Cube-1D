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

void Indicators::setupBuzzer(uint8_t buzzerPin)
{
    m_buzzerPin = buzzerPin;
    m_buzzerEnabled = true;
    pinMode(m_buzzerPin, OUTPUT);

    buzzerMutex = xSemaphoreCreateMutex(); // Create buzzer mutex

    // quick chirp
    controlBuzzer(tones[6].frequency, 50);
}

void Indicators::setupRGBLed(uint8_t ledNeo)
{
    m_ledNeo = ledNeo;
    m_RGBLedEnabled = true;
    pinMode(m_ledNeo, OUTPUT);

    rgbLedMutex = xSemaphoreCreateMutex(); // Create the RGB LED mutex

    // quick test
    controlRGBLed(colours[1].value, 50); // Green
}

void Indicators::showCriticalError()
{
    ESP_LOGI("Indicators", "Showing Critical Error!");

    uint16_t duration = 100; // ms

    for (int i = 0; i < 3; i++)
    {
        controlBuzzer(tones[0].frequency, duration);
        controlRGBLed(colours[0].value, duration); // Red
        vTaskDelay(pdMS_TO_TICKS(duration));       // off period
    }
}

void Indicators::showWarning()
{
    ESP_LOGI("Indicators", "Showing Warning!");

    uint16_t duration = 50; // ms

    controlBuzzer(tones[3].frequency, duration);
    controlRGBLed(colours[7].value, duration); // Yellow
}

void Indicators::showSuccess()
{
    ESP_LOGI("Indicators", "Showing Success!");

    uint16_t duration = 50; // ms

    for (int i = 0; i < 3; i++)
    {
        controlBuzzer(tones[i].frequency, duration);
        controlRGBLed(colours[1].value, duration); // Green
        vTaskDelay(pdMS_TO_TICKS(duration));
    }
}

void Indicators::showAllGood()
{
    // ESP_LOGI("Indicators", "Showing All Good!");

    uint16_t duration = 50; // ms

    controlRGBLed(colours[1].value, duration);
}

void Indicators::showAllOff()
{
    ESP_LOGI("Indicators", "Turning off all Indicators!");

    controlRGBLed(0, 0);
}

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

bool Indicators::checkStatusEither()
{
    SemaphoreGuard guard(statusMutex);
    if (guard.acquired())
    {
        return m_buzzerEnabled || m_RGBLedEnabled; // !!! Do we need to do mutexing, probably
    }
    return false;
}

bool Indicators::checkStatusBuzzer()
{
    SemaphoreGuard guard(statusMutex);
    if (guard.acquired())
    {
        return m_buzzerEnabled; // !!! Do we need to do mutexing, probably
    }
    return false;
}

bool Indicators::checkStatusLed()
{
    SemaphoreGuard guard(statusMutex);
    if (guard.acquired())
    {
        return m_RGBLedEnabled; // !!! Do we need to do mutexing, probably
    }
    return false;
}