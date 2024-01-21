#include "websocket_handler.h"
// #include "Debug.h"
extern AsyncWebServer server;
WebSocketHandler::WebSocketHandler() : asyncWebSocket("/ws")
{
}
WebSocketHandler::~WebSocketHandler()
{
    delete &asyncWebSocket;
}
void WebSocketHandler::cleanupClients()
{
    asyncWebSocket.cleanupClients();
}

void WebSocketHandler::initWebSocket(AsyncWebServer &asyncWebServer)
{

    asyncWebSocket.onEvent(std::bind(&WebSocketHandler::websocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    asyncWebServer.addHandler(&asyncWebSocket);
}

void WebSocketHandler::notifyClients(String sensorReadings)
{
    asyncWebSocket.textAll(sensorReadings);
}
void WebSocketHandler::handleWebSocketMessage(AsyncWebSocket *server, AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
{
    // data packet
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len)
    {
        // the whole message is in a single frame and we got all of it's data
        Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
        if (info->opcode == WS_TEXT)
        {
            data[len] = 0;
            Serial.printf("%s\n", (char *)data);
        }
        else
        {
            for (size_t i = 0; i < info->len; i++)
            {
                Serial.printf("%02x ", data[i]);
            }
            Serial.printf("\n");
        }
        if (info->opcode == WS_TEXT)
            client->text("I got your text message");
        else
            client->binary("I got your binary message");
    }
    else
    {
        // message is comprised of multiple frames or the frame is split into multiple packets
        if (info->index == 0)
        {
            if (info->num == 0)
                Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
            Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
        }

        Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);
        if (info->message_opcode == WS_TEXT)
        {
            data[len] = 0;
            Serial.printf("%s\n", (char *)data);
        }
        else
        {
            for (size_t i = 0; i < len; i++)
            {
                Serial.printf("%02x ", data[i]);
            }
            Serial.printf("\n");
        }

        if ((info->index + len) == info->len)
        {
            Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
            if (info->final)
            {
                Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                if (info->message_opcode == WS_TEXT)
                    client->text("I got your text message");
                else
                    client->binary("I got your binary message");
            }
        }
    }
}

void WebSocketHandler::websocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("ws[%s][%u] connected from %s\n", server->url(), client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
        break;
    case WS_EVT_DATA:
        this->handleWebSocketMessage(server, client, arg, data, len);
        break;
    case WS_EVT_PONG:
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
        break;
    case WS_EVT_ERROR:
        Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
        break;
    }
}

String WebSocketHandler::getVelocityReadings(String msg, float *velocity)
{
    readings["act"] = msg;
    JsonArray velocityArrayV = readings.createNestedArray("vel");
    for (int i = 0; i < 25; i++)
    {
        velocityArrayV.add(int(velocity[i]));
    }
    String jsonString;
    serializeJson(readings, jsonString);
    // Serial.println(jsonString);
    readings.clear();
    return jsonString;
}
String WebSocketHandler::getCoordinateReadings(int16_t *coordinateX, int16_t *coordinateY)
{
    JsonArray velocityArrayV = readings.createNestedArray("vel");
    JsonArray velocityArrayX = readings.createNestedArray("col_x");
    JsonArray velocityArrayY = readings.createNestedArray("col_y");
    for (int i = 0; i < 25; i++)
    {
        velocityArrayX.add(int(coordinateX[i]));
        velocityArrayY.add(int(coordinateY[i]));
    }
    String jsonString;
    serializeJson(readings, jsonString);
    // Serial.println(jsonString);
    readings.clear();
    return jsonString;
}
String WebSocketHandler::getReadings(InferenceWindow inferenceWindow, String msg)
{
    readings["act"] = msg;
    JsonArray velocityArrayV = readings.createNestedArray("vel");
    JsonArray velocityArrayX = readings.createNestedArray("col_x");
    JsonArray velocityArrayY = readings.createNestedArray("col_y");
    for (int i = 0; i < 25; i++)
    {
        velocityArrayV.add(int(inferenceWindow.velocityInput[0][i]));
        velocityArrayX.add(int(inferenceWindow.coordinateX[0][i]));
        velocityArrayY.add(int(inferenceWindow.coordinateY[0][i]));
    }
    String jsonString;
    serializeJson(readings, jsonString);
    // Serial.println(jsonString);
    readings.clear();
    return jsonString;
}
void initWiFi(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("->WiFi connected");
    Serial.println("->IP address: ");
    Serial.println(WiFi.localIP());
}
void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
    Serial.println("SPIFFS mounted successfully");
}
void connectServer()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });

    server.serveStatic("/", SPIFFS, "/");

    // Start server
    server.begin();
}