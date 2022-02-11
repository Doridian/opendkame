#include <Arduino.h>
#include <EEPROM.h>

#include "codes.h"
#include "rf.h"
#include "cc1101.h"
#include "config.h"

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  transmitInit();
}

void serialLoop() {
  if (!Serial || !Serial.available()) {
    return;
  }

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
  else if (str.equals("cc1101version"))
  {
    Serial.print("CC1101VERSION ");
    cc1101.select();
    Serial.println(CC1101_MAIN.SpiReadStatus(CC1101_VERSION));
  }
  else if (str.equals("index"))
  {
    Serial.print("INDEX ");
    Serial.println(getTransmitCodeIndex());
  }
}

void loop() {
  serialLoop();
  transmitLoop();
  delay(10);
}
