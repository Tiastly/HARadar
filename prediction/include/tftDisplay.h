#ifndef TFTDISPLAY_H
#define TFTDISPLAY_H
#include <TFT_eSPI.h>

#define LABEL_OFFSET 20
extern TFT_eSPI tft;
const int TARGET_COLOR[3] = {TFT_SKYBLUE, TFT_PINK, TFT_YELLOW};
void showMessage(String msg[], int target);
void drawPosition(int16_t x, int16_t y, int i);
void tftUpdate();

#endif