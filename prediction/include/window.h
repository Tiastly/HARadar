#ifndef _WINDOW_H
#define _WINDOW_H

#include "HLK_LD2450.h"
#define WIN_SIZE 25 // Each 2.5s make a prediction
static const char *ACTION_LABEL[5] = {"Sitting", "Stretching", "Walking", "Jumping", "Running"};
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
float *getInferenceVelocity(struct InferenceWindow *InferenceWindow, uint8_t i);
int resultAnalyse(float *result, int16_t *coordinateX, int16_t *coordinateY);
bool checkCoordinate(int16_t *coordinateX, int16_t *coordinateY, int idx);
void addVelocities(struct InferenceWindow *InferenceWindow, int16_t velocity, uint8_t i);
void addCoordinates(struct InferenceWindow *InferenceWindow, int16_t x, int16_t y, uint8_t i);
// float *getVelocities(struct InferenceWindow *InferenceWindow, uint8_t i);
// int16_t *getCoordinateXs(struct InferenceWindow *InferenceWindow, uint8_t i);
// int16_t *getCoordinateYs(struct InferenceWindow *InferenceWindow, uint8_t i);
#endif