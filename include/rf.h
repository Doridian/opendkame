#pragma once

extern bool rxDebugEnable;

void transmitInit();
void transmitLoop();
void transmitNextCode();
void receiveISR();
void transmitLearningCode();
uint32_t transmitGetCodeIndex();
void transmitSetCodeIndex(uint32_t newIndex);
