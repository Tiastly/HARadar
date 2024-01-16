#include <TensorFlowLite_ESP32.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <TFT_eSPI.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "model.h"
#include "window.h"
#include "HLK_LD2450.h"

const char *ACTION_LABEL[5] = {"Sitting", "Stretching", "Walking", "Jumping", "Running"};
const int TARGET_COLOR[3] = {TFT_BLUE, TFT_CYAN, TFT_GREEN};
int LABEL_OFFSET = 20;
namespace
{
  tflite::ErrorReporter *error_reporter = nullptr;
  const tflite::Model *model = nullptr;
  tflite::MicroInterpreter *interpreter = nullptr;
  TfLiteTensor *input = nullptr;
  TfLiteTensor *output = nullptr;

  constexpr int kTensorArenaSize = 30 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
} // namespace

const char *ssid = "TP_LINK_114514"; // Wifi SSID
const char *password = "1145141919"; // Wifi Password

const char *mqtt_server = "192.168.1.103";
unsigned int mqtt_port = 1883;

const char *subTopic = "esp32/pub";

#define ld2450_rx 16
#define ld2450_tx 17

HLK_LD2450 ld2450(ld2450_rx, ld2450_tx, &Serial);
struct InferenceWindow InferenceWindow = initInferenceWindow();
WiFiClient wifiClient;
PubSubClient client(wifiClient);
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
String msg;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("received from Laptop: [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
}

void connectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", "", ""))
    {
      Serial.println("-> MQTT client connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("-> try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void connectWifi()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    // WiFi.reconnect();
  }
  Serial.println("");
  Serial.println("->WiFi connected");
  Serial.println("->IP address: ");
  Serial.println(WiFi.localIP());
}

void drawX(int x, int y)
{
  tft.drawLine(x - 5, y - 5, x + 5, y + 5, TFT_WHITE);
  tft.drawLine(x - 5, y + 5, x + 5, y - 5, TFT_WHITE);
}

void showMessage(String msg)
{
  // Clear the screen areas
  tft.fillRect(0, 0, tft.width(), 20, TFT_BLACK);
  // tft.fillRect(0, 20, tft.width(), tft.height()-20, TFT_BLUE);
  uint8_t td = tft.getTextDatum();            // Get current datum
  tft.setTextDatum(TC_DATUM);                 // Set new datum
  tft.drawString(msg, tft.width() / 2, 2, 2); // Message in font 2
  tft.setTextDatum(td);                       // Restore old datum
}

void drawPosition(int16_t x, int16_t y, int i)
{
  int scale_factor_x = (tft.width() * 1000 / 2000);
  int scale_factor_y = (tft.height() * 1000 / 2000.0);

  x = ((x + 1000) * scale_factor_x) / 1000;
  y = ((y + 1000) * scale_factor_y) / 1000;
  tft.fillCircle(x, y, 2, TARGET_COLOR[i]);
}
void setup()
{
  Serial.begin(256000);
  // tflite
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  model = tflite::GetModel(quantized_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk)
  {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);
  // mqtt
  Serial.print("connecting to WIFI : ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  connectWifi();

  // client.setServer(mqtt_server, mqtt_port);
  // client.setCallback(callback);
  // connectMQTT();

  // Serial.print("Subscribe topic:");
  // Serial.print(subTopic);
  // // susbscribe
  // client.subscribe(subTopic);

  // st7735
  tft.begin(); // initialize
  tft.setRotation(0);
}

void loop()
{
  // unsigned long startTime = millis();

  ld2450.processTarget();
  for (int i = 0; i < 1; i++)
  {
    drawPosition(ld2450.getCoordinateX(i), ld2450.getCoordinateY(i), i);
    addInferenceWindow(&InferenceWindow, ld2450, i);
  }
  if (isInferenceWindowFull(&InferenceWindow))
  {
    float *input_data = getInferenceWindow(&InferenceWindow, 0);
    for (int i = 0; i < WIN_SIZE; i++)
    {
      input->data.f[i] = input_data[i];
      // memcpy(input->data.f, getInferenceWindow(&InferenceWindow, 0), WIN_SIZE); coredump
    }

    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
      TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed on x: %f\n",
                           static_cast<double>(0));
      return;
    }

    Serial.print("output:\n");
    float *output_data = output->data.f;
    int idx = resultAnalyse(output_data, InferenceWindow.coordinateX[0], InferenceWindow.coordinateY[0]);
    if (idx != -1)
    {
      msg = ACTION_LABEL[idx];
    }
    else
    {
      msg = "Unknown";
    }
    Serial.printf(" max_label: %s\n", msg);
    int xw = tft.width() / 2; // xw, yh is middle of screen
    int yh = tft.height() / 2;

    showMessage(msg);
    delay(1000);
    tft.fillRect(0, LABEL_OFFSET, tft.width(), tft.height() - LABEL_OFFSET, TFT_RED);
    tft.drawLine(tft.width() / 2, LABEL_OFFSET, tft.width() / 2, tft.height(), TFT_WHITE);                             // vertical line
    tft.drawLine(0, tft.height() / 2 + LABEL_OFFSET / 2, tft.width(), tft.height() / 2 + LABEL_OFFSET / 2, TFT_WHITE); // horizontal line
    // client.loop();
  }
}
