/*
  AD9959.h - Library for writing yo AD9959 with Arduino
  2019 Noah Schlossberger
*/

// ensure this library description is only included once
#ifndef AD9959_h
#define AD9959_h

// include SPI library for writing to DDS
#include <SPI.h>

class AD9959
{
  // user-accessible "public" interface
  public:
    AD9959(int resetPin, int IO_UpdatePin, int csbPin, int SDIO3Pin);
    void initialize();
    void initializeDDS();
    void channelSel(int chNO);
    void resetDDS();
    void pulseUpdate();
    void writeFreq(long ftw);
    void resetCommunication();
    void spiwrite(byte data[], int numBytes, SPISettings standSets);


  // private interface - serial handling
  private:
    int _ResetPin;
    int _IO_UpdatePin;
    int _CsbPin;
    int _SDIO3Pin;
    SPISettings _standSets;
};
#endif
