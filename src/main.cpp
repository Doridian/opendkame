#include <Adafruit_TinyUSB.h>
#include <Arduino.h>

#include "codes.h"
#include "rf.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);
  transmitInit();
}

void loop() {
  String str = Serial.readStringUntil('\n');
  str.trim();
  if (str.length() > 0) {
    Serial.print("Sending...");
    transmitNextCode();
    Serial.println(" Done!");
  }
}
