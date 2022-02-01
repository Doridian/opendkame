#include <Arduino.h>

#include "codes.h"
#include "rf.h"

#define SYMBOL_LENGTH 400
#define SYMBOL_COUNT (PREFIX_BITS + ((SUFFIX_BITS + CODE_BITS) * 3))
#define REPEATS 1
#define REPEAT_DELAY 10000
#define PIN_GDO0 13

uint8_t TX_DATA_BASE[SYMBOL_COUNT];

static void _transmitData(const uint8_t *txData)
{
    for (uint16_t i = 0; i < SYMBOL_COUNT; i++)
    {
        digitalWrite(PIN_GDO0, txData[i]);
        delayMicroseconds(SYMBOL_LENGTH);
    }
}

static void encodeCodeRaw(uint8_t *data, uint32_t len, uint64_t raw)
{
    for (uint32_t i = 0; i < len; i++)
    {
        data[i] = (raw >> (len - i - 1)) & 1;
    }
}

static void encodeCodePWM(uint8_t *data, uint32_t len, uint64_t raw)
{
    for (uint32_t i = 0; i < len; i++)
    {
        const int j = i * 3;
        const bool on = (raw >> (len - i - 1)) & 1;
        data[j] = 1;
        data[j + 1] = !on;
        data[j + 2] = 0;
    }
}

int transmitCodeIndex()
{
    static uint16_t idx = 0;
    return idx++;
}

void transmitInit()
{
    encodeCodeRaw(TX_DATA_BASE, PREFIX_BITS, PREFIX);
    encodeCodePWM(TX_DATA_BASE + SYMBOL_COUNT - (SUFFIX_BITS * 3), SUFFIX_BITS, SUFFIX);
}

void transmitNextCode()
{
    uint8_t txData[SYMBOL_COUNT];
    memcpy(txData, TX_DATA_BASE, SYMBOL_COUNT);

    encodeCodePWM(txData + PREFIX_BITS, CODE_BITS, CODES[transmitCodeIndex()]);

    // cc1101.beginTransmission();
    for (uint8_t i = 0; i < REPEATS; i++)
    {
        _transmitData(txData);
        digitalWrite(PIN_GDO0, LOW);
        delayMicroseconds(REPEAT_DELAY);
    }
    // cc1101.endTransmission();
}
