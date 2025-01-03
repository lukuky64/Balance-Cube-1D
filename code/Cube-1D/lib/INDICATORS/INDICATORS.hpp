#ifndef INDICATORS_HPP
#define INDICATORS_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "Arduino.h"
#include <vector>

struct Tone
{
    char name;     // Musical note
    int frequency; // Frequency in Hz
};

struct Colour
{
    char name; // Colour identifier
    int value; // Colour Hex code
};

class INDICATORS
{
public:
    INDICATORS();
    ~INDICATORS();

    void setupBuzzer(uint8_t buzzerPin);
    void setupRGBLed(uint8_t ledR, uint8_t ledG, uint8_t ledB);

    bool controlBuzzer(int frequency, int duration);
    bool controlRGBLed(int hexValue, int duration);

    void showCriticalError();
    void showWarning();
    void showSuccess();
    void showAllGood();
    void showIdle();

    bool checkStatus();

private:
    SemaphoreHandle_t buzzerMutex; // Mutex for buzzer
    SemaphoreHandle_t rgbLedMutex; // Mutex for RGB LED

    bool m_buzzerEnabled = false;
    bool m_RGBLedEnabled = false;

    uint8_t m_buzzerPin;

    uint8_t m_ledR;
    uint8_t m_ledG;
    uint8_t m_ledB;

    // C major scale for buzzer
    const std::vector<Tone> tones = {
        {'C', 523},
        {'D', 587},
        {'E', 659},
        {'F', 698},
        {'G', 784},
        {'A', 880},
        {'B', 987}};

    const std::vector<Colour> colours = {
        {'R', 0xFF0000},
        {'G', 0x00FF00},
        {'B', 0x0000FF},
        {'Y', 0xFFFF00},
        {'M', 0xFF00FF},
        {'C', 0x00FFFF},
        {'W', 0xFFFFFF},
        {'O', 0xFFA500}};
};

#endif // INDICATORS_HPP