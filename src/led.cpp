#include "led.h"
#include "config.h"

#include <NeoPixelConnect.h>

NeoPixelConnect ledPixels(PIN_NEOPIXEL, 1);

void ledSetRGB(uint8_t r, uint8_t g, uint8_t b)
{
    ledPixels.neoPixelFill(r, g, b, true);
}

void ledSetOff()
{
    ledSetRGB(0, 0, 0);
}

void ledInit()
{
    pinMode(PIN_NEOPIXEL_POWER, OUTPUT);
    digitalWrite(PIN_NEOPIXEL_POWER, HIGH);

    ledSetRGB(0, 0, 255);
}
