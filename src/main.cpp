#include <Arduino.h>

#include "codes.h"
#include "rf.h"

void setup() {
  Serial.begin(115200);
  transmitInit();
}

void loop() {
  Serial.readStringUntil('\n');
  transmitNextCode();
}
