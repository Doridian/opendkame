#include <Arduino.h>
#include <SPI.h>

#include "cc1101.h"
#include "cc1101_defs.h"
#include "config.h"

CC1101Transceiver cc1101(PIN_CLK, PIN_MISO, PIN_MOSI, PIN_CS, PIN_GDO0,
                         PIN_GDO2);

const SPISettings spiSettings(4000000, MSBFIRST, SPI_MODE0);

#define SPI_BEGIN() { SPI.beginTransaction(spiSettings); digitalWrite(this->CSN, LOW); while(digitalRead(this->MISO)); }
#define SPI_END() { digitalWrite(this->CSN, HIGH); SPI.endTransaction(); }

CC1101Transceiver::CC1101Transceiver(byte SCK, byte MISO, byte MOSI, byte CSN,
                                     byte GDO0, byte GDO2) {
  this->SCK = SCK;
  this->MISO = MISO;
  this->MOSI = MOSI;
  this->CSN = CSN;
  this->GDO0 = GDO0;
  this->GDO2 = GDO2;
}

void CC1101Transceiver::setup() {  // initialize the SPI pins
  pinMode(this->SCK, OUTPUT);
  pinMode(this->MOSI, OUTPUT);
  pinMode(this->MISO, INPUT);
  pinMode(this->CSN, OUTPUT);

  pinMode(this->GDO0, OUTPUT);
  pinMode(this->GDO2, INPUT);

  digitalWrite(this->GDO0, LOW);

  digitalWrite(this->SCK, HIGH);
  digitalWrite(this->MOSI, LOW);
  digitalWrite(this->CSN, HIGH);

  SPI.setCS(this->CSN);
  SPI.setRX(this->MISO);
  SPI.setTX(this->MOSI);
  SPI.setSCK(this->SCK);
  SPI.begin();

  delayMicroseconds(10);

  // Set up the CC1101
  digitalWrite(this->CSN, LOW);
  delayMicroseconds(10);
  digitalWrite(this->CSN, HIGH);
  delayMicroseconds(40);
  digitalWrite(this->CSN, LOW);
  while(digitalRead(this->MISO));
  this->spiStrobe(CC1101_SRES);
  while(digitalRead(this->MISO));
  digitalWrite(this->CSN, HIGH);
  // End reset procedure

  this->spiWriteReg(CC1101_IOCFG0,0x2E);  //GDO0 Output Pin Configuration
  this->spiWriteReg(CC1101_IOCFG1,0x2E);  //GDO1 Output Pin Configuration
  this->spiWriteReg(CC1101_IOCFG2,0x0D);  //GDO2 Output Pin Configuration
  this->spiWriteReg(CC1101_PKTCTRL0,0x32);//Packet Automation Control
  this->spiWriteReg(CC1101_FSCTRL1,0x06); //Frequency Synthesizer Control
  this->spiWriteReg(CC1101_FREQ2,0x0C);   //Frequency Control Word, High Byte
  this->spiWriteReg(CC1101_FREQ1,0x3B);   //Frequency Control Word, Middle Byte
  this->spiWriteReg(CC1101_FREQ0,0x13);   //Frequency Control Word, Low Byte
  this->spiWriteReg(CC1101_MDMCFG4,0xF6); //Modem Configuration
  this->spiWriteReg(CC1101_MDMCFG3,0x93); //Modem Configuration
  this->spiWriteReg(CC1101_MDMCFG2,0x30); //Modem Configuration
  this->spiWriteReg(CC1101_MDMCFG1,0x00); //Modem Configuration
  this->spiWriteReg(CC1101_DEVIATN,0x15); //Modem Deviation Setting
  this->spiWriteReg(CC1101_MCSM0,0x18);   //Main Radio Control State Machine Configuration
  this->spiWriteReg(CC1101_FOCCFG,0x16);  //Frequency Offset Compensation Configuration
  this->spiWriteReg(CC1101_FREND0,0x11);  //Front End TX Configuration
  this->spiWriteReg(CC1101_FSCAL3,0xE9);  //Frequency Synthesizer Calibration
  this->spiWriteReg(CC1101_FSCAL2,0x2A);  //Frequency Synthesizer Calibration
  this->spiWriteReg(CC1101_FSCAL1,0x00);  //Frequency Synthesizer Calibration
  this->spiWriteReg(CC1101_FSCAL0,0x1F);  //Frequency Synthesizer Calibration
  this->spiWriteReg(CC1101_TEST2,0x81);   //Various Test Settings
  this->spiWriteReg(CC1101_TEST1,0x35);   //Various Test Settings

  this->endTransmission();
}

void CC1101Transceiver::beginTransmission() {
  this->select();
  digitalWrite(this->GDO0, LOW);
  this->spiStrobe(CC1101_SIDLE);
  this->spiStrobe(CC1101_STX);
}

void CC1101Transceiver::endTransmission() {
  this->select();
  digitalWrite(this->GDO0, LOW);
  this->spiStrobe(CC1101_SIDLE);
  this->spiStrobe(CC1101_SRX);
  this->spiStrobe(CC1101_SIDLE);
  this->spiStrobe(CC1101_SRX);
}

void CC1101Transceiver::spiStrobe(const uint8_t data) {
  SPI_BEGIN();
  SPI.transfer(data);
  SPI_END();
}

void CC1101Transceiver::spiWriteReg(const uint8_t addr, const uint8_t data) {
  SPI_BEGIN();
  SPI.transfer(addr);
  SPI.transfer(data);
  SPI_END();
}

uint8_t CC1101Transceiver::spiReadStatus(const uint8_t addr) 
{
  uint8_t value;
  SPI_BEGIN();
  SPI.transfer(addr | CC1101_READ_BURST);
  value = SPI.transfer(0);
  SPI_END();
  return value;
}

uint8_t CC1101Transceiver::spiReadReg(const uint8_t addr) 
{
  uint8_t value;
  SPI_BEGIN();
  SPI.transfer(addr | CC1101_READ_SINGLE);
  value = SPI.transfer(0);
  SPI_END();
  return value;
}

byte CC1101Transceiver::getTXPin() { return this->GDO0; }
void CC1101Transceiver::select() {
  Serial.println(this->spiReadStatus(CC1101_VERSION), HEX);
  Serial.println(this->spiReadReg(CC1101_MDMCFG0), HEX);
  Serial.println(this->spiReadReg(CC1101_MDMCFG1), HEX);
  Serial.println(this->spiReadReg(CC1101_MDMCFG2), HEX);
}
