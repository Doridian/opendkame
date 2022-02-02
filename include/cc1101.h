#pragma once

#include <Arduino.h>

class CC1101Transceiver {
public:
  CC1101Transceiver(byte SCK, byte MISO, byte MOSI, byte CSN, byte GDO0,
                    byte GDO2);
  void setup();
  void beginTransmission();
  void endTransmission();
  byte getTXPin();

protected:
  byte SCK;
  byte MISO;
  byte MOSI;
  byte CSN;
  byte GDO0;
  byte GDO2;
  byte moduleNumber;

  void spiStrobe(const uint8_t data);
  void spiWriteReg(const uint8_t addr, const uint8_t data);
  uint8_t spiReadReg(const uint8_t addr);
  uint8_t spiReadStatus(const uint8_t addr);
};

extern CC1101Transceiver cc1101;
