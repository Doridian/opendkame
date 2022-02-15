#include <Arduino.h>

#include "serial.h"
#include "rf.h"
#include "cc1101.h"

String serialBuffer;

void serialInit()
{
    serialBuffer.reserve(128);
    Serial.begin(115200);
}

static void serialHandleCommand()
{
    serialBuffer.trim();
    serialBuffer.toLowerCase();

    Serial.print(serialBuffer);
    Serial.print(" ");

    if (serialBuffer.equals("learn"))
    {
        transmitLearningCode();
    }
    else if (serialBuffer.equals("open"))
    {
        transmitNextCode();
    }
    else if (serialBuffer.equals("rxdebug"))
    {
        Serial.print(!rxDebugEnable);
        rxDebugEnable = !rxDebugEnable;
    }
    else if (serialBuffer.equals("cc1101version"))
    {
        cc1101.select();
        Serial.print(CC1101_MAIN.SpiReadStatus(CC1101_VERSION));
    }
    else if (serialBuffer.equals("getindex"))
    {
        Serial.print(transmitGetCodeIndex());
    }
    else if (serialBuffer.startsWith("setindex "))
    {
        uint32_t newIndex = serialBuffer.substring(9).toInt();
        transmitSetCodeIndex(newIndex);
        Serial.print(transmitGetCodeIndex());
    }
    else
    {
        Serial.print("UNKNOWN COMMAND");
    }

    Serial.println();
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
