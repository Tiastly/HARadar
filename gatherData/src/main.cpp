#include "HLK_LD2450.h"

#define ld2450_rx 2
#define ld2450_tx 3

HLK_LD2450 ld2450(ld2450_rx, ld2450_tx, &Serial1);

void setup() {
  Serial.begin(256000);
}
void loop() {
  
  //   Sitting
  //   Waving
  //   Walking 
  //   Jumping 
  //   Running 
  ld2450.processTarget();
  Serial.print("0,");

  Serial.print(ld2450.getTargetX(0));
  Serial.print(",");
  Serial.print(ld2450.getTargetY(0));
  Serial.print(",");
  Serial.print(ld2450.getSpeed(0));
  Serial.print("\n");
  // delay(10);
}