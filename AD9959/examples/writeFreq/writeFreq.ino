#include <AD9959.h>

// AD9959 Example
// by Noah Schlossberger <http://www.noahschlossberger.com>

// Demostrates how to write to an AD9959 chip with the AD9959 library
// Created Jan 2019

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Pin connection:
  IMPORTANT!! - use level shifters to go from Arduino 5v <-> DDS 3.3v
  csbPin -> CSB
  IO_UpdatePin -> IO Update
  resetPin -> resetPin
  SDIO3Pin -> SDIO3
  (implicit pins chosen by SPI library [varies with arduino model, check library reference])
  Pin 11 (MOSI) -> SDIO0
  Pin 13 (SCK) ->  SCLK
  Pin 12 (MISO) -> SDIO2 (not used)
  GND -> POWERDOWN
  When using the development board make sure to apply 3.3 and 1.8v power supplies to ALL required ports (both terminals and SMA)
  Also, by default only the filter outputs will work
 */

//select pin configuration (can be anything except 11, 12, 13)
const int csbPin = 7;
const int IO_UpdatePin = 2;
const int resetPin = 10;
const int SDIO3Pin = 3;



void setup()
{
  AD9959 myDDS = AD9959(resetPin, IO_UpdatePin, csbPin, SDIO3Pin);
  myDDS.initialize();           // initialize
}

void loop()
{
  {
    myDDS.channelSel(0);             //select a channel (0,1,2,3) to write to.  4 selects all channels
    long testFrequencyTuningWord = 214750000;
    myDDS.writeFreq(testFrequencyTuningWord);  //write a new frequency to the selected channel
    myDDS.pulseUpdate();        //update output to the new frequency
  }
  delay(1000);
  {
    long testFreq = 214750000;
    myDDS.writeFreq(testFreq);
    myDDS.pulseUpdate();
  }
  delay(1000);
}
