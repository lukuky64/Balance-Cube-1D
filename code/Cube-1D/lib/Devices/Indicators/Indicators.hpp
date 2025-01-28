#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "SemaphoreGuard.hpp"
#include "esp32-hal-rgb-led.h"

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

class Indicators
{
public:
    Indicators();
    ~Indicators();

    void setupBuzzer(uint8_t buzzerPin);
    void setupRGBLed(uint8_t ledNeo);

    bool controlBuzzer(int frequency, int duration);
    bool controlRGBLed(int hexValue, int duration);

    void showCriticalError();
    void showWarning();
    void showSuccess();
    void showAllGood();
    void showAllOff();

    bool checkStatus();
    bool checkStatusBuzzer();
    bool checkStatusLed();

private:
    SemaphoreHandle_t buzzerMutex; // Mutex for buzzer
    SemaphoreHandle_t rgbLedMutex; // Mutex for RGB LED

    SemaphoreHandle_t statusMutex;
    bool m_buzzerEnabled = false;
    bool m_RGBLedEnabled = false;

    uint8_t m_buzzerPin;

    uint8_t m_ledNeo;

    // C major scale for buzzer
    const std::vector<Tone> tones = {
        {'C', 523},
        {'D', 587},
        {'E', 659},
        {'F', 698},
        {'G', 784},
        {'A', 880},
        {'B', 987}};

    // 0xRRGGBB
    const std::vector<Colour> colours = {
        {'R', 0xFF0000},
        {'G', 0x00FF00},
        {'B', 0x0000FF},
        {'Y', 0xFFFF00},
        {'M', 0xFF00FF},
        {'C', 0x00FFFF},
        {'W', 0xFFFFFF},
        {'O', 0xFF7B00}};

    static constexpr const char *TAG = "Indicators";
};