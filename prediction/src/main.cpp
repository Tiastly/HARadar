
#include "window.h"
#include "inference.h"
#include "HLK_LD2450.h"

// #define TFT_MODULE
#define WEB_MODULE

#if defined(TFT_MODULE)
#include "tftDisplay.h"
#endif
#if defined(WEB_MODULE)
#include "websocket_handler.h"
#endif

const char *ssid = "TP_LINK_114514"; // Wifi SSID
const char *password = "1145141919"; // Wifi Password

HLK_LD2450 ld2450(ld2450_rx, ld2450_tx, &Serial);
struct InferenceWindow inferenceWindow = initInferenceWindow();
String msg;

unsigned long previousMillis = 0;
unsigned long interval = 30000;

#if defined(TFT_MODULE)
TFT_eSPI tft = TFT_eSPI();
#endif
#if defined(WEB_MODULE)
AsyncWebServer server(80);
WebSocketHandler webSocket;
#endif

void setup()
{
  Serial.begin(256000);
  // tflite
  initTfLite();

#if defined(WEB_MODULE)
  initWiFi(ssid, password);
  initSPIFFS();
  webSocket.initWebSocket(server);
  connectServer();
#endif
#if defined(TFT_MODULE)
  tft.begin(); // initialize
  tft.setRotation(0);
#endif
}

void loop()
{
  ld2450.processTarget();
  for (int i = 0; i < 1; i++)
  {
#if defined(WEB_MODULE)
    // too fast
#endif
#if defined(TFT_MODULE)
    drawPosition(ld2450.getCoordinateX(i), ld2450.getCoordinateY(i), i);
#endif
    addInferenceWindow(&inferenceWindow, ld2450, i);
  }
  if (isInferenceWindowFull(&inferenceWindow))
  {
    float *input_data = getInferenceVelocity(&inferenceWindow, 0);
    float *output_data = makeInference(input_data, WIN_SIZE);
    if (output_data)
    {
      int idx = resultAnalyse(output_data, inferenceWindow.coordinateX[0], inferenceWindow.coordinateY[0]);
      if (idx != -1)
        msg = ACTION_LABEL[idx];
      else
        msg = "Unknown";
      Serial.printf(" max_label: %s\n", msg);
    }
    else
      Serial.println("Error with inference");
#if defined(TFT_MODULE)
    tftUpdate(msg);
#endif
#if defined(WEB_MODULE)
    String payload = webSocket.getReadings(inferenceWindow, msg);

    webSocket.notifyClients(payload);

    // webSocket.cleanupClients();
#endif
  }
}
