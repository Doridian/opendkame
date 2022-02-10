#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <EEPROM.h>

#include "codes.h"
#include "rf.h"
#include "cc1101.h"

void setup() {
  EEPROM.begin(256);
  Serial.begin(115200);
  while (!Serial) delay(100);
  transmitInit();
}

void loop() {
  String str = Serial.readStringUntil('\n');
  str.trim();
  str.toLowerCase();
  if (str.equals("learn"))
  {
    Serial.println("LEARN");
    transmitLearningCode();
  }
  else if (str.equals("open"))
  {
    Serial.println("OPEN");
    transmitNextCode();
  }
  else if (str.equals("rxdebug"))
  {
    Serial.print("RXDEBUG ");
    Serial.println(!rxDebugEnable);
    rxDebugEnable = !rxDebugEnable;
  }
  else if (str.equals("test"))
  {
    Serial.print("TEST ");
    cc1101.select();
    Serial.println(CC1101_MAIN.SpiReadStatus(CC1101_VERSION));
  }
  delay(100);
}
