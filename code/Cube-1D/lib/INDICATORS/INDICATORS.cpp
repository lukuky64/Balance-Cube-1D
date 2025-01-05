#include "INDICATORS.hpp"

INDICATORS::INDICATORS()
{
}

INDICATORS::~INDICATORS()
{
    // Disable the buzzer and RGB LED

    m_buzzerEnabled = false;
    m_RGBLedEnabled = false;

    // Delete the mutexes
    vSemaphoreDelete(buzzerMutex);
    vSemaphoreDelete(rgbLedMutex);
}

void INDICATORS::setupBuzzer(uint8_t buzzerPin)
{
    m_buzzerPin = buzzerPin;
    m_buzzerEnabled = true;
    pinMode(m_buzzerPin, OUTPUT);

    buzzerMutex = xSemaphoreCreateMutex(); // Create buzzer mutex

    // quick test
    controlBuzzer(tones[6].frequency, 50);
}

void INDICATORS::setupRGBLed(uint8_t ledNeo)
{
    m_ledNeo = ledNeo;
    m_RGBLedEnabled = true;
    pinMode(m_ledNeo, OUTPUT);

    rgbLedMutex = xSemaphoreCreateMutex(); // Create the RGB LED mutex

    // quick test
    controlRGBLed(colours[1].value, 50); // Green
}

void INDICATORS::showCriticalError()
{
    ESP_LOGI("INDICATORS", "Showing Critical Error!");

    uint16_t duration = 100; // ms

    for (int i = 0; i < 3; i++)
    {
        controlBuzzer(tones[0].frequency, duration);
        controlRGBLed(colours[0].value, duration); // Red
        vTaskDelay(pdMS_TO_TICKS(duration));
    }
}

void INDICATORS::showWarning()
{
    ESP_LOGI("INDICATORS", "Showing Warning!");

    uint16_t duration = 50; // ms

    controlBuzzer(tones[3].frequency, duration);
    controlRGBLed(colours[7].value, duration); // Yellow
}

void INDICATORS::showSuccess()
{
    ESP_LOGI("INDICATORS", "Showing Success!");

    uint16_t duration = 50; // ms

    for (int i = 0; i < 3; i++)
    {
        controlBuzzer(tones[i].frequency, duration);
        controlRGBLed(colours[1].value, duration); // Green
        vTaskDelay(pdMS_TO_TICKS(duration));
    }
}

void INDICATORS::showAllGood()
{
    ESP_LOGI("INDICATORS", "Showing All Good!");

    uint16_t duration = 50; // ms

    controlRGBLed(colours[1].value, duration);
}

void INDICATORS::showAllOff()
{
    ESP_LOGI("INDICATORS", "Turning off all indicators!");

    controlRGBLed(0, 0);
}

bool INDICATORS::controlBuzzer(int frequency, int duration)
{

    if (m_buzzerEnabled)
    {
        // Take the mutex before accessing the buzzer
        if (xSemaphoreTake(buzzerMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            tone(m_buzzerPin, frequency);
            vTaskDelay(pdMS_TO_TICKS(duration));
            noTone(m_buzzerPin);

            // Release the mutex
            xSemaphoreGive(buzzerMutex);
            return true;
        }
    }
    return false;
}

bool INDICATORS::controlRGBLed(int hexValue, int duration)
{
    if (m_RGBLedEnabled)
    {

        SemaphoreGuard guard(rgbLedMutex);
        if (guard.acquired())
        {
            uint8_t red = (hexValue >> 16) & 0xFF;
            uint8_t green = (hexValue >> 8) & 0xFF;
            uint8_t blue = hexValue & 0xFF;

            neopixelWrite(RGB_BUILTIN, red, green, blue); // Red

            vTaskDelay(pdMS_TO_TICKS(duration));

            neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Red

            return true;
        }
    }
    return false;
}

bool INDICATORS::checkStatus()
{
    return m_buzzerEnabled || m_RGBLedEnabled; // !!! Do we need to do mutexing, probably
}