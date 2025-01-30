#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <esp_log.h>

class WebSocketServer
{
public:
    WebSocketServer(uint16_t port = 8080);
    void begin();
    void loop();
    void sendMessage(const String &message);
    bool isConnected();

private:
    WebSocketsServer webSocket;

    const char *ap_ssid = "ESP32_AP";     // 🔹 WiFi network name (ESP32 creates this)
    const char *ap_password = "12345678"; // 🔹 WiFi password (optional)
};
