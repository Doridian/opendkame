#include <Adafruit_TinyUSB.h>
#include <Arduino.h>

#include "codes.h"
#include "rf.h"

void setup() {
  Serial.begin(115200);
  //while (!Serial) delay(100);
  transmitInit();
}

void loop() {
  String str = Serial.readStringUntil('\n');
  str.trim();
  str.toLowerCase();
  if (str.equals("learn"))
  {
    transmitLearningCode();
  }
  else if (str.equals("open"))
  {
    transmitNextCode();
  }
  delay(100);
}
