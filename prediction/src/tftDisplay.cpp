#include "tftDisplay.h"

void showMessage(String msg[], int Max_Target)
{
  // Clear the screen areas
  int32_t stringWidth = tft.width() / Max_Target;
  for (int i = 0; i < Max_Target; i++)
  {

    tft.fillRect(stringWidth * i, 0, stringWidth, 20, TFT_BLACK);

    uint8_t td = tft.getTextDatum(); // Get current datum
    tft.setTextDatum(TC_DATUM);      // Set new datum
    tft.setTextColor(TARGET_COLOR[i]);
    tft.drawString(msg[i], stringWidth * i + stringWidth / 2, 2, 2); // Message in font 2
    tft.setTextDatum(td);                                            // Restore old datum
  }
  tft.fillRect(stringWidth * Max_Target, 0, tft.width() - stringWidth * Max_Target, 20, TFT_BLACK);//fill the rest of the screen with black
}

void drawPosition(int16_t x, int16_t y, int i)
{
  int scale_factor_x = (tft.width() * 1000 / 2000);
  int scale_factor_y = ((tft.height() - LABEL_OFFSET) * 1000 / 2000.0);

  x = ((x + 1000) * scale_factor_x) / 1000;
  y = ((y + 1000) * scale_factor_y) / 1000 + LABEL_OFFSET;
  tft.fillCircle(x, y, 2, TARGET_COLOR[i]);
}
void tftUpdate()
{
  tft.fillRect(0, LABEL_OFFSET, tft.width(), tft.height() - LABEL_OFFSET, TFT_RED);
  tft.drawLine(tft.width() / 2, LABEL_OFFSET, tft.width() / 2, tft.height(), TFT_WHITE);                             // vertical line
  tft.drawLine(0, tft.height() / 2 + LABEL_OFFSET / 2, tft.width(), tft.height() / 2 + LABEL_OFFSET / 2, TFT_WHITE); // horizontal line
}
