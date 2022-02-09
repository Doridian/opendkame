#pragma once

extern bool rxDebugEnable;

void transmitInit();
void transmitNextCode();
void receiveISR();
void transmitLearningCode();
