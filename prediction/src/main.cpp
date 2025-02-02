
#include "window.h"
#include "inference.h"
#include "HLK_LD2450.h"

const char *ssid = "TP_LINK_114514"; // Wifi SSID
const char *password = "1145141919"; // Wifi Password

HLK_LD2450 ld2450(ld2450_rx, ld2450_tx, &Serial);
struct InferenceWindow inferenceWindow = initInferenceWindow();
String action[3];

// #define TFT_MODULE
#define WEB_MODULE
#define GAME_MODULE

#if defined(TFT_MODULE)
#include "tftDisplay.h"
TFT_eSPI tft = TFT_eSPI();
#endif

#if defined(WEB_MODULE)
#include "websocket_handler.h"
AsyncWebServer server(80);
WebSocketHandler webSocket;
#endif

#if defined(GAME_MODULE)
const bool game = true;
#else
const bool game = false;
#endif

#define Max_Target 1
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
  for (int i = 0; i < Max_Target; i++)
  {
#if defined(WEB_MODULE)
    // too fast
#endif
#if defined(TFT_MODULE)
    drawPosition(ld2450.getCoordinateX(i), ld2450.getCoordinateY(i), i);
#endif
    addInferenceWindow(&inferenceWindow, ld2450, i);
  }
  if (inferenceWindow.isFull)
  {
    for (int i = 0; i < Max_Target; i++)
    {

      float *input_data = getInferenceVelocity(&inferenceWindow, i);
      float *output_data = makeInference(input_data, WIN_SIZE);
      if (output_data)
      {
        int idx = resultAnalyse(output_data, inferenceWindow.coordinateX[i], inferenceWindow.coordinateY[i],game);
        if (idx != -1)
          action[i] = ACTION_LABEL[idx];
        else
          action[i] = "Unknown";
      }
      else
      {
        Serial.println("Error with inference");
      }
      Serial.printf("velocity: [%d]\n", i + 1);
      for (int elem : inferenceWindow.velocityInput[i])
      {
        Serial.printf("%d,", elem);
      }
      Serial.printf("\n[Info]%s\n", action[i]);
    }
#if defined(TFT_MODULE)
    showMessage(action, Max_Target);
    tftUpdate();
#endif
#if defined(WEB_MODULE)
    String payload = webSocket.getReadings(inferenceWindow, action, Max_Target);
    webSocket.notifyClients(payload);

    // webSocket.cleanupClients();
#endif
  }

  // tftUpdate();
  inferenceWindow.isFull = false;
}
