#pragma once

#include <Arduino.h>
#include <functional> // For std::function

// #if ARDUINO_USB_CDC_ON_BOOT
// #define SERIAL_INTERFACE USBSerial
// #else
// #define SERIAL_INTERFACE Serial
// #endif

#define SERIAL_INTERFACE Serial

class CLI
{
public:
    using CommandCallback = std::function<void(const char *)>;

    // Constructor: starts serial at the given baud rate
    explicit CLI(unsigned long baud = 115200);

    // Register a command ID + callback
    void addCommand(char cmdID, CommandCallback callback);

    // Continuously poll serial for incoming characters and handle commands
    void run();

private:
    // A small struct to keep track of individual commands
    struct Command
    {
        char id;
        CommandCallback callback;
    };

    static const int MAX_COMMANDS = 10;
    static const int MAX_BUFFER_SIZE = 64;

    Command _commandList[MAX_COMMANDS];
    int _commandCount = 0;

    char _rxBuffer[MAX_BUFFER_SIZE];
    int _rxIndex = 0;

    // Helper function to parse and handle the buffer
    void handleBuffer();
};
