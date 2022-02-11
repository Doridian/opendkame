#include <Arduino.h>
#include <EEPROM.h>

#include "codes.h"
#include "rf.h"
#include "cc1101.h"
#include "config.h"
#include "serial.h"

void setup()
{
    EEPROM.begin(EEPROM_SIZE);
    serialInit();
    transmitInit();
}

void loop()
{
    serialLoop();
    transmitLoop();
    delay(10);
}
