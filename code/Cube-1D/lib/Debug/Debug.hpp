#ifndef DEBUG_HPP
#define DEBUG_HPP

#ifndef TARGET_ENV_NATIVE

#include <Arduino.h>

#ifdef ENABLE_DEBUG

#ifdef USE_USBSERIAL
#define _SERIAL USBSerial
#else
#define _SERIAL Serial
#endif // USE_USBSERIAL

#define DBG(...)                      \
    do                                \
    {                                 \
        _SERIAL.print("[");           \
        _SERIAL.print(__FUNCTION__);  \
        _SERIAL.print("(): ");        \
        _SERIAL.print(__LINE__);      \
        _SERIAL.print(" ] ");         \
        _SERIAL.println(__VA_ARGS__); \
    } while (0)

#define INITIALISE_DBG(...)         \
    do                              \
    {                               \
        _SERIAL.begin(__VA_ARGS__); \
    } while (0)

#else // ENABLE_DEBUG

#define DBG(...)
#define INITIALISE_DBG(...)

#endif // ENABLE_DEBUG

#else // TARGET_ENV_NATIVE

#define DBG(...)
#define INITIALISE_DBG(...)

#endif // TARGET_ENV_NATIVE

#endif // DEBUG_HPP