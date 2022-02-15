#include <Arduino.h>
#include <EEPROM.h>

#include "codes.h"
#include "rf.h"
#include "config.h"
#include "cc1101.h"
#include "rfconfig.h"
#include "led.h"

#define EXTRACT_BIT(raw, len, i) ((raw >> (len - i - 1)) & 1)

bool rxDebugEnable = false;
bool txNextCodeEnable = false;

uint32_t transmitCodeIndex = 0;

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

static inline void transmitSaveCodeIndex()
{
    EEPROM.put(EEPROM_CODE_INDEX, transmitCodeIndex);
    EEPROM.commit();
}

static uint32_t transmitGetNextCodeIndex()
{
    transmitCodeIndex++;
    if (transmitCodeIndex >= CODE_COUNT)
    {
        transmitCodeIndex = 0;
    }
    transmitSaveCodeIndex();
    return transmitCodeIndex;
}

uint32_t transmitGetCodeIndex()
{
    return transmitCodeIndex;
}

void transmitSetCodeIndex(uint32_t newIndex)
{
    transmitCodeIndex = newIndex;
    transmitSaveCodeIndex();
}

void transmitInit()
{
    EEPROM.get(EEPROM_CODE_INDEX, transmitCodeIndex);
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
    txNextCodeEnable = true;
}

static void realTransmitNextCode()
{
    ledSetRGB(255, 0, 0);

    uint8_t txData[SYMBOL_COUNT];
    memcpy(txData, TX_DATA_BASE, SYMBOL_COUNT);

    encodeCodePWM(txData + PREFIX_BITS, CODE_BITS, CODES[transmitGetNextCodeIndex()]);

    cc1101.beginTransmission();
    for (uint8_t i = 0; i < REPEATS; i++)
    {
        _transmitData(txData);
        digitalWrite(PIN_GDO0, LOW);
        delayMicroseconds(REPEAT_DELAY);
    }
    cc1101.endTransmission();

    ledSetOff();
}

void transmitLoop()
{
    if (txNextCodeEnable)
    {
        txNextCodeEnable = false;
        realTransmitNextCode();
    }
}

void transmitLearningCode()
{
    ledSetRGB(255, 255, 255);

#ifdef TX_FREQ
    cc1101.txFreq = RX_FREQ;
#endif

    cc1101.beginTransmission();
    while (true)
    {
        for (uint16_t j = 0; j < LEARNING_CODE_LENGTH; j++)
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

    ledSetOff();
}

void receiveISR()
{
    static unsigned long lastChange = 0;
    static bool lastState = LOW;
    static uint32_t recvState = LEARNING_CODE_START;
    static unsigned long lastCorrect = 0;

    const bool state = digitalRead(PIN_GDO2);
    if (state == lastState)
    {
        return;
    }

    const unsigned long now = micros();

    const unsigned long delayTime = now - lastChange;

    const bool correctState = LEARNING_CODE[recvState] < 0;
    const int32_t correctVal = abs(LEARNING_CODE[recvState]);

    const int32_t minDelay = correctVal - (correctVal / 4);
    const int32_t maxDelay = correctVal + (correctVal / 4);

    if (rxDebugEnable)
    {
        Serial.print(minDelay);
        Serial.print(" ");
        Serial.print(maxDelay);
        Serial.print(" ");
        Serial.print(correctState);
        Serial.print(" ");
        Serial.println(delayTime);
    }

    if (delayTime > maxDelay || delayTime < minDelay)
    {
        recvState = LEARNING_CODE_START;
    }
    else
    {
        recvState++;
        if (rxDebugEnable)
        {
            Serial.print("RS ");
            Serial.println(recvState);
        }

        if (recvState >= LEARNING_CODE_LENGTH - 1)
        {
            recvState = LEARNING_CODE_START;
            if (now - lastCorrect >= RX_COOLDOWN)
            {
                if (Serial)
                {
                    Serial.println("Got correct RF -> Transmitting code!");
                }

                ledSetRGB(0, 255, 0);
                transmitNextCode();
            }
            lastCorrect = now;
        }
    }

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
