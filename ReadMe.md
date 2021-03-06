# Write to AD9959 with Arduino
A simple arduino file that controls an AD9959 device with the SPI library

I wrote this because I could not find an already existing library that I could get to work.
To install, download and copy the "AD9959" folder into your arduino libraries folder, usually Documents/Arduino/libraries on windows. Then you should be able to go to the editor's File->Examples->AD9959->writeFreq to see how it works.

To get this to work you will need to check the following:

1. Ensure that all jumpers are moved to the manual control position

2. Ensure you have supplied power to ALL 1.8 and 3.3V power ports on the DDS. If you are using the development board as I am, you must supply to ALL of the screw terminals as well as ONE of the SMA terminals (the rest of the SMA terminals are in series).

3. You will need a reference clock that is at least twice the frequency you wish to generate, which is connected to the REF CLK sma connector. I use a .6V pk-pk signal with no issues.

4. Make sure you are connected to the DAC filter outputs instead of the DAC outputs.

5. Connect the arduino ports to the pins as instructed in the arduino file.
csbPin -> CSB <br/>
IO_UpdatePin -> IO Update <br/>
resetPin -> resetPin <br/>
SDIO3Pin -> SDIO3 <br/>
(implicit pins chosen by SPI library [varies with arduino model, check SPI library [Refernence](https://www.arduino.cc/en/reference/SPI)] for your model)<br/>
Pin 11 (MOSI) -> SDIO0 <br/>
Pin 13 (SCK) ->  SCLK <br/>
Pin 12 (MISO) -> SDIO2 (not used) <br/>
  
You MUST use level shifter to go from the 5v arduino outputs to 3.3v inputs on the DDS. I use resistors to make a voltage divider as shown below. 

![alt text](https://github.com/nkschlos/Write-to-AD9959-with-Arduino/blob/master/images/schemeit-project.png?raw=true)

Keep in mind that on the Dev board, the SDIO ports have internal 1k resistors to ground (screencap from the datasheet below), so if you are using voltage dividors you need to account for this (hency my extra resistor on the SDIO dividors).

![alt text](https://github.com/nkschlos/Write-to-AD9959-with-Arduino/blob/master/images/1kresistors.PNG?raw=true)

I used a header to house the wires so it could be easily plugged into the development board, and I put the voltage dividers on a protoboard.

![alt text](https://github.com/nkschlos/Write-to-AD9959-with-Arduino/blob/master/images/board.jpg?raw=true)

