#include <Arduino.h>

#include "codes.h"
#include "rf.h"
#include "config.h"
#include "cc1101.h"
#include "rfconfig.h"

#define EXTRACT_BIT(raw, len, i) ((raw >> (len - i - 1)) & 1)

bool rxDebugEnable = false;

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
        data[i] = EXTRACT_BIT(raw, len, i);
    }
}

static void encodeCodePWM(uint8_t *data, uint32_t len, uint64_t raw)
{
    for (uint32_t i = 0; i < len; i++)
    {
        const int j = i * 3;
        const bool on = EXTRACT_BIT(raw, len, i);
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
    cc1101.setup(RX_FREQ);
#ifdef TX_FREQ
    cc1101.txFreq = TX_FREQ;
    cc1101.rxFreq = RX_FREQ;
#endif
}

void transmitNextCode()
{
    uint8_t txData[SYMBOL_COUNT];
    memcpy(txData, TX_DATA_BASE, SYMBOL_COUNT);

    encodeCodePWM(txData + PREFIX_BITS, CODE_BITS, CODES[transmitCodeIndex()]);

    cc1101.beginTransmission();
    for (uint8_t i = 0; i < REPEATS; i++)
    {
        _transmitData(txData);
        digitalWrite(PIN_GDO0, LOW);
        delayMicroseconds(REPEAT_DELAY);
    }
    cc1101.endTransmission();
}

void transmitLearningCode()
{
#ifdef TX_FREQ
    cc1101.txFreq = RX_FREQ;
#endif

    cc1101.beginTransmission();
    for (uint8_t i = 0; i < REPEATS; i++)
    {
        for (uint16_t j = 0; j < sizeof(LEARNING_CODE); j++)
        {
            digitalWrite(PIN_GDO0, LEARNING_CODE[j] > 0);
            delayMicroseconds(abs(LEARNING_CODE[j]));
        }
        digitalWrite(PIN_GDO0, LOW);
        delayMicroseconds(REPEAT_DELAY);
    }
    cc1101.endTransmission();

#ifdef TX_FREQ
    cc1101.txFreq = TX_FREQ;
#endif
}

void receiveISR()
{
    static unsigned long lastChange = 0;
    static bool lastState = LOW;

    const bool state = digitalRead(PIN_GDO2);
    if (state == lastState)
    {
        return;
    }

    const unsigned long now = micros();

    if (rxDebugEnable)
    {
        Serial.print(lastState ? "HIGH" : "LOW");
        Serial.print(" -> ");
        Serial.print(state ? "HIGH" : "LOW");
        Serial.print(" ");
        Serial.println(now - lastChange);
    }

    lastChange = now;
    lastState = state;
}
