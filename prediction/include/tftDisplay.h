#ifndef TFTDISPLAY_H
#define TFTDISPLAY_H
#include <TFT_eSPI.h>

#define LABEL_OFFSET 20
extern TFT_eSPI tft;
const int TARGET_COLOR[3] = {TFT_BLUE, TFT_CYAN, TFT_GREEN};
void showMessage(String msg);
void drawPosition(int16_t x, int16_t y, int i);
void tftUpdate(String msg);

#endif