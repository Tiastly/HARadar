
#include "window.h"
#include "inference.h"
#include "HLK_LD2450.h"

HLK_LD2450 ld2450(ld2450_rx, ld2450_tx, &Serial);
struct InferenceWindow inferenceWindow = initInferenceWindow();
String action[3];

// #define TFT_MODULE
#define GAME_MODULE

#if defined(TFT_MODULE)
#include "tftDisplay.h"
TFT_eSPI tft = TFT_eSPI();
#endif

#if defined(GAME_MODULE)
bool game = true;
#else
bool game = false;
#endif

#define Max_Target 1

void setup()
{
  Serial.begin(256000);
  // tflite
  initTfLite();

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
        int idx = resultAnalyse(output_data, inferenceWindow.coordinateX[i], inferenceWindow.coordinateY[i], game);
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
      Serial.printf("\n[INFO]%s\n", action[i].c_str());
    }
#if defined(TFT_MODULE)
    showMessage(action, Max_Target);
    tftUpdate();
#endif
  }

  // tftUpdate();
  inferenceWindow.isFull = false;
}
