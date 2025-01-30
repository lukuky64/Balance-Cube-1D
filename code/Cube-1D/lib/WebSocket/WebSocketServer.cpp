#include "WebSocketServer.hpp"

WebSocketServer::WebSocketServer(uint16_t port) : webSocket(port) {}

void WebSocketServer::begin()
{
    // Start ESP32 as an Access Point
    WiFi.softAP(ap_ssid, ap_password);
    ESP_LOGI("WebSocket", "ESP32 AP IP: %s", WiFi.softAPIP().toString().c_str());

    // Start WebSocket Server
    webSocket.begin();
    webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t *payload, size_t length)
                      {
        if (type == WStype_CONNECTED)
        {
            ESP_LOGI("WebSocket", "Client [%u] connected!\n", num);
        }
        else if (type == WStype_TEXT)
        {
            ESP_LOGI("WebSocket", "Received: %s\n", payload);
        } });
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
