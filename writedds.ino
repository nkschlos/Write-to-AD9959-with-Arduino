
#include <SPI.h>
/*
  ***********Written by Noah Schlossberger in Jan 2019
  
  Place your code inside void loop()
  A complete writing of the frequency looks like this:

  channelSel( <channel to write frequency to as int>);   // only required if writing to a different channel than before, 4 writes to all channels
  writeFreq( <frequency tuning word formatted as long);  // the value can be computed as ftw = round(2^32 * <desiredfreq> /<refclockfreq>)
  pulseUpdate();                                         // tells the DDS to output the new written frequency

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


//define pins
const int csbPin = 7;
const int IO_UpdatePin = 6;
const int resetPin = 10;
const int SDIO3Pin = 3;

//define settings
SPISettings standSets(14000, MSBFIRST, SPI_MODE0);

void setup()
{ ///////////////////////////// initialize (always runs first)
  //define pinmodes
  pinMode(IO_UpdatePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(csbPin, OUTPUT);
  //prewrite logic levels
  digitalWrite(csbPin, HIGH);
  digitalWrite(IO_UpdatePin, LOW);
  //initialize
  SPI.begin();
  resetDDS();
  digitalWrite(csbPin, LOW); //tells the board to start listening
  //initializeDDS();
}
void loop()
{
  //place code here. simple write example that writes to all channels is shown below
  channelSel(4);
  {
    long testfrequency = 21475000;
    writeFreq(testfrequency);
  }
  pulseUpdate();
}

void initializeDDS() //writes all of the static settings to the DDS
{
  resetDDS();
  digitalWrite(csbPin, LOW);
  delay(1);

  channelSel(4);

  //FR1 Function Register 1
  {
    byte valsToWrite[] = {B00000000, B00000000, B00000000, B00000000};
    spiwrite(valsToWrite, 4, standSets);
  }


  //FR2 Function Register 2
  {
    byte valsToWrite[] = {B00000010, B10100000, B00000000};
    spiwrite(valsToWrite, 3, standSets);
  }

  //CFR Channel Function Register (Single Tone, AM, FM, etc)
  {
    byte valsToWrite[] = {B00000011, B10000000, B00000011, B00000000};
    spiwrite(valsToWrite, 4, standSets);
  }
}

void  channelSel(int chNO)
{
  byte chans[] = {B0010000, B00100000, B01000000, B10000000, B11110000};
  {
    byte valsToWrite[] = {B00000000, chans[chNO]};
    spiwrite(valsToWrite, 2, standSets);
  }
}

void writeFreq(long ftw)
{
  {
    //CTW0 Channel Frequency Tuning Word 0 (Frequency Selection)
    byte writeCFTW0_1 = (byte )((ftw >> 24) & 0xff);
    byte writeCFTW0_2 = (byte )((ftw >> 16) & 0xff);
    byte writeCFTW0_3 = (byte )((ftw >> 8) & 0xff);
    byte writeCFTW0_4 = (byte )(ftw & 0xff);

    byte valsToWrite[] = {B00000100, writeCFTW0_1, writeCFTW0_2, writeCFTW0_3, writeCFTW0_4};
    spiwrite(valsToWrite, 5,  standSets);
  }
}

void spiwrite(byte data[], int numBytes, SPISettings standSets)  //writes an array of bytes to the DDS board. Takes an array of bytes to write, the length of the array, and SPI settings.
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

void resetCommunication() //resets DDS internal communication (using before new command adds protection against previous bad writes)
{
  digitalWrite(csbPin, HIGH);
  digitalWrite(SDIO3Pin, HIGH);
  digitalWrite(SDIO3Pin, LOW);
  digitalWrite(csbPin, LOW);
}

void resetDDS() //master resets the DDS
{
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
}

void pulseUpdate() //pulses an update to renew the frequency
{
  digitalWrite(IO_UpdatePin, HIGH);
  digitalWrite(IO_UpdatePin, LOW);
}
