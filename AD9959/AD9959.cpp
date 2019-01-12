/*
  AD9959.h -
  Jan 2019, Noah Schlossberger
*/


#include <SPI.h>
// include this library's description file
#include "AD9959.h"
//define fast pin manipulation for speedup of manual control

#define portOfPin(P)\
  (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define ddrOfPin(P)\
  (((P)>=0&&(P)<8)?&DDRD:(((P)>7&&(P)<14)?&DDRB:&DDRC))
#define pinOfPin(P)\
  (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P)((uint8_t)(P>13?P-14:P&7))
#define pinMask(P)((uint8_t)(1<<pinIndex(P)))

#define pinAsInput(P) *(ddrOfPin(P))&=~pinMask(P)
#define pinAsInputPullUp(P) *(ddrOfPin(P))&=~pinMask(P);digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P))|=pinMask(P)
#define digitalLow(P) *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P)((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)((*(pinOfPin(P))& pinMask(P))==0)
#define digitalState(P)((uint8_t)isHigh(P))

// Constructor /////////////////////////////////////////////////////////////////
// Function that handles the creation and setup of instances
AD9959::AD9959(int resetPin, int IO_UpdatePin, int csbPin, int SDIO3Pin)
{
  SPISettings standSets(1400000, MSBFIRST, SPI_MODE0);
  _standSets = standSets;
  _ResetPin = resetPin;
  _IO_UpdatePin=IO_UpdatePin;
  _CsbPin=csbPin;
  _SDIO3Pin=SDIO3Pin;
}

void AD9959::initialize()
{
  //define pins as outputs
  pinAsOutput(_IO_UpdatePin);
  pinAsOutput(_ResetPin);
  pinAsOutput(_CsbPin);
  pinAsOutput(_SDIO3Pin);
  //prewrite logic levels
  digitalHigh(_CsbPin);
  //initialize
  SPI.begin();
  resetDDS();
  initializeDDS();
  digitalLow(_CsbPin); //tells the board to start listening
}
void AD9959::initializeDDS() //writes all of the static settings to the DDS
{
  resetDDS();
  digitalLow(_CsbPin);
  delay(1);

  //FR1 Function Register 1
  {
    byte valsToWrite[] = {B00000000, B00000000, B00000000, B00000000};
    spiwrite(valsToWrite, 4, _standSets);
  }


  //FR2 Function Register 2
  {
    byte valsToWrite[] = {B00000010, B10100000, B00000000};
    spiwrite(valsToWrite, 3, _standSets);
  }

  //CFR Channel Function Register (Single Tone, AM, FM, etc)
  {
    byte valsToWrite[] = {B00000011, B00000000, B00000011, B00000000};
    spiwrite(valsToWrite, 4, _standSets);
  }
}

void AD9959::channelSel(int chNO) //sets channel to write to
{
  byte chans[] = {B0010000, B00100000, B01000000, B10000000, B11110000};
  {
    byte valsToWrite[] = {B00000000, chans[chNO]};
    spiwrite(valsToWrite, 2, _standSets);
  }
}
void AD9959::resetDDS() //master resets the DDS
{
  digitalHigh(_ResetPin);
  digitalLow(_ResetPin);
}
void AD9959::pulseUpdate() //pulses an update to renew the frequency
{
  digitalHigh(_IO_UpdatePin);
  digitalLow(_IO_UpdatePin);
}
void AD9959::writeFreq(long ftw) //writes a new frequency to the selected channel
{
  {
    //CTW0 Channel Frequency Tuning Word 0 (Frequency Selection)
    byte writeCFTW0_1 = (byte )((ftw >> 24) & 0xff);//B01110000;
    byte writeCFTW0_2 = (byte )((ftw >> 16) & 0xff);//B00111101;
    byte writeCFTW0_3 = (byte )((ftw >> 8) & 0xff);//B00001010;
    byte writeCFTW0_4 = (byte )(ftw & 0xff);//B10100100;
    byte valsToWrite[] = {B00000100, writeCFTW0_1, writeCFTW0_2, writeCFTW0_3, writeCFTW0_4};
    spiwrite(valsToWrite, 5,  _standSets);
  }
}
void AD9959::resetCommunication() //resets DDS internal communication (using before new command adds protection against previous bad writes)
{
  digitalHigh(_CsbPin);
  digitalHigh(_SDIO3Pin);
  digitalLow(_SDIO3Pin);
  digitalLow(_CsbPin);
}

void AD9959::spiwrite(byte data[], int numBytes, SPISettings standSets)  //writes an array of bytes to the DDS board. Takes an array of bytes to write, the length of the array, and SPI settings.
{
  resetCommunication();
  SPI.beginTransaction(standSets);
  for (int i = 0; i < numBytes; i++) //for all bytes in byte array
  {
    //delay(1);
    SPI.transfer(data[i]);//write byte
  }
  SPI.endTransaction();
}
