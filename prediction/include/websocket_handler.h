// websocket_handler.h

#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <ArduinoJSON.h>

#include "window.h"
class WebSocketHandler
{
public:
    WebSocketHandler();
    ~WebSocketHandler();
    void initWebSocket(AsyncWebServer &asyncWebServer);
    void websocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void notifyClients(String sensorReadings);
    String getReadings(InferenceWindow inferenceWindow,String msg[],int target);
    String getVelocityReadings(String msg,float* velocity);
    String getCoordinateReadings(int16_t* coordinateX,int16_t* coordinateY);
    void handleWebSocketMessage(AsyncWebSocket *server, AsyncWebSocketClient *client,void *arg, uint8_t *data, size_t len);
    void cleanupClients();
private:
    AsyncWebSocket asyncWebSocket; 
    StaticJsonDocument<1024*4> readings;
};
void initWiFi(const char *ssid, const char *password);
void initSPIFFS();
void connectServer();
#endif // WEBSOCKET_HANDLER_H
