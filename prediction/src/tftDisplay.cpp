#include "tftDisplay.h"

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
void tftUpdate(String msg){
    showMessage(msg);
    tft.fillRect(0, LABEL_OFFSET, tft.width(), tft.height() - LABEL_OFFSET, TFT_RED);
    tft.drawLine(tft.width() / 2, LABEL_OFFSET, tft.width() / 2, tft.height(), TFT_WHITE);                             // vertical line
    tft.drawLine(0, tft.height() / 2 + LABEL_OFFSET / 2, tft.width(), tft.height() / 2 + LABEL_OFFSET / 2, TFT_WHITE); // horizontal line
}
