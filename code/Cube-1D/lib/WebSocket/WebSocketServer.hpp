#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <esp_log.h>
#include "Params.hpp"

const int MAX_COMMANDS = 10; // Define MAX_COMMANDS

class WebSocketServer
{
public:
    using CommandCallback = std::function<void(const char *)>;

    WebSocketServer(uint16_t port = 8080);
    void begin();
    void loop();
    void sendMessage(const String &message);
    bool isConnected();
    void addCommand(char cmdID, CommandCallback callback);
    void setVariable(const char *arg);
    void commandHandler(const String &command);
    void pingMsg();

private:
    struct Command
    {
        char id;
        CommandCallback callback;
    };

    WebSocketsServer webSocket;

    const char *ap_ssid = "BALANCE_CUBE_1D"; // 🔹 WiFi network name
    const char *ap_password = "12345678";    // 🔹 WiFi password, minimum 8 characters

    Command _commandList[MAX_COMMANDS];
    int _commandCount = 0;
};
