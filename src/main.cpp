#include <Arduino.h>
#include <EEPROM.h>

#include "codes.h"
#include "rf.h"
#include "cc1101.h"
#include "config.h"

String serialBuffer;

void setup()
{
  EEPROM.begin(EEPROM_SIZE);
  serialBuffer.reserve(128);
  Serial.begin(115200);
  transmitInit();
}

void serialHandleCommand()
{
  serialBuffer.trim();
  serialBuffer.toLowerCase();
  if (serialBuffer.equals("learn"))
  {
    Serial.println("LEARN");
    transmitLearningCode();
  }
  else if (serialBuffer.equals("open"))
  {
    Serial.println("OPEN");
    transmitNextCode();
  }
  else if (serialBuffer.equals("rxdebug"))
  {
    Serial.print("RXDEBUG ");
    Serial.println(!rxDebugEnable);
    rxDebugEnable = !rxDebugEnable;
  }
  else if (serialBuffer.equals("cc1101version"))
  {
    Serial.print("CC1101VERSION ");
    cc1101.select();
    Serial.println(CC1101_MAIN.SpiReadStatus(CC1101_VERSION));
  }
  else if (serialBuffer.equals("index"))
  {
    Serial.print("INDEX ");
    Serial.println(transmitGetCodeIndex());
  }
  else
  {
    Serial.print("UNKNOWN ");
    Serial.println(serialBuffer);
  }
}

void serialLoop()
{
  while (Serial && Serial.available())
  {
    char c = Serial.read();
    if (c == '\n' || c == '\r')
    {
      if (serialBuffer.length() > 0)
      {
        serialHandleCommand();
        serialBuffer = "";
      }
    }
    else
    {
      serialBuffer += c;
    }
  }
}

void loop()
{
  serialLoop();
  transmitLoop();
  delay(10);
}
