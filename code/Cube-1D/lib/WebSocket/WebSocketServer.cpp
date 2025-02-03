#include "WebSocketServer.hpp"

WebSocketServer::WebSocketServer(uint16_t port) : webSocket(port) {}

void WebSocketServer::begin()
{
    // Start ESP32 as an Access Point
    WiFi.softAP(ap_ssid, ap_password);
    ESP_LOGI("WebSocket", "IP: %s", WiFi.softAPIP().toString().c_str());

    // Start WebSocket Server
    webSocket.begin();
    webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t *payload, size_t length)
                      {
        if (type == WStype_CONNECTED)
        {
            ESP_LOGI("WebSocket", "Client [%u] connected!\n", num);
        }
        else if (type == WStype_TEXT)
        {
            ESP_LOGI("WebSocket", "Received: %s\n", payload);
            this->commandHandler((char *)payload);
        } });

    // Set a variable
    addCommand('S', [this](const char *arg)
               { this->setVariable(arg); });

    // Wipe all modified variables and restart
    addCommand('R', [](const char *arg)
               { Params::wipeSettings(); });

    // ping command
    addCommand('P', [this](const char *arg)
               { this->pingMsg(); });

    // get a variable
}

void WebSocketServer::pingMsg()
{
    sendMessage("Pong");
    ESP_LOGI("WebSocket", "Pong");
}

void WebSocketServer::loop()
{
    webSocket.loop();
}

void WebSocketServer::sendMessage(const String &message)
{
    webSocket.broadcastTXT(message.c_str(), message.length());
}

bool WebSocketServer::isConnected()
{
    return WiFi.softAPgetStationNum() > 0; // Returns true if at least one client is connected
}

void WebSocketServer::commandHandler(const String &command)
{
    // The first character in the command is the command ID
    char cmdID = command[0];

    // The rest of the string (after the first character) is the command argument
    const char *arg = (command.length() > 1) ? command.c_str() + 1 : "";

    // Search for a matching command
    for (int i = 0; i < _commandCount; i++)
    {
        if (_commandList[i].id == cmdID)
        {
            // If found, call its callback with the argument
            _commandList[i].callback(arg);
            return;
        }
    }

    // If no match, print an error
    ESP_LOGW("WebSocket", "Unknown command: %c", cmdID);
}

void WebSocketServer::setVariable(const char *arg)
{
    const char *delimiter = strchr(arg, '=');
    if (!delimiter)
    {
        ESP_LOGW("WebSocket", "Invalid format. Use varName=value");
        return;
    }

    // Trim leading spaces from the parameter name
    String varName = String(arg).substring(0, delimiter - arg);
    varName.trim(); // <-- Removes leading and trailing spaces

    String valueStr = String(arg).substring(delimiter - arg + 1);
    valueStr.trim(); // <-- Also removes any accidental spaces

    if (Params::exists(varName.c_str())) // Check if param exists
    {
        if (valueStr.indexOf('.') >= 0)
        {
            float fValue = valueStr.toFloat();
            Params::savePreference(varName.c_str(), fValue);
            ESP_LOGI("WebSocket", "Stored float preference: %s = %f", varName.c_str(), fValue);
        }
        else
        {
            unsigned int iValue = valueStr.toInt();
            Params::savePreference(varName.c_str(), iValue);
            ESP_LOGI("WebSocket", "Stored integer preference: %s = %d", varName.c_str(), iValue);
        }
    }
    else
    {
        ESP_LOGW("WebSocket", "Parameter %s does not exist!", varName.c_str());
    }
}

void WebSocketServer::addCommand(char cmdID, CommandCallback callback)
{
    // Prevent overflow of the command array
    if (_commandCount < MAX_COMMANDS)
    {
        _commandList[_commandCount].id = cmdID;
        _commandList[_commandCount].callback = callback;
        _commandCount++;

        // this shouldn't really be here

        ESP_LOGI("WebSocket", "Added command. Type '%c' paramName=value to set preferences.", cmdID);
    }
    else
    {
        ESP_LOGI("WebSocket", "Command list is full!");
    }
}