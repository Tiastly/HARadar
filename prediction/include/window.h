#ifndef _WINDOW_H
#define _WINDOW_H

#define WIN_SIZE 25 // Each 2.5s make a prediction
#include <Arduino.h>
#include "HLK_LD2450.h"

struct InferenceWindow
{
  float velocityInput[3][WIN_SIZE];
  int16_t coordinateX[3][WIN_SIZE];
  int16_t coordinateY[3][WIN_SIZE];
  size_t idx;
  bool isFull;
};
struct InferenceWindow initInferenceWindow();
void addInferenceWindow(struct InferenceWindow *InferenceWindow, HLK_LD2450 frame, uint8_t i);
bool isInferenceWindowFull(struct InferenceWindow *InferenceWindow);
float *getInferenceWindow(struct InferenceWindow *InferenceWindow, uint8_t i);
int resultAnalyse(float *result, int16_t *coordinateX, int16_t *coordinateY);
bool checkCoordinate(int16_t *coordinateX, int16_t *coordinateY, int idx);
void addVelocity(struct InferenceWindow *InferenceWindow, int16_t velocity, uint8_t i);
void addCoordinate(struct InferenceWindow *InferenceWindow, int16_t x, int16_t y, uint8_t i);
#endif