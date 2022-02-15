#include <Arduino.h>
#include <EEPROM.h>

#include "codes.h"
#include "rf.h"
#include "cc1101.h"
#include "config.h"
#include "serial.h"
#include "led.h"

void setup()
{
    ledInit();

    EEPROM.begin(EEPROM_SIZE);
    serialInit();
    transmitInit();

    ledSetOff();
}

void loop()
{
    serialLoop();
    transmitLoop();
    delay(10);
}
