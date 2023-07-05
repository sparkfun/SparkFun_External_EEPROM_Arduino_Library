/*
  Read and write settings and calibration data to an external I2C EEPROM
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 11th, 2019
  License: This code is public domain but you buy me a beer if you use this 
  and we meet someday (Beerware license).
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14764

  This example demonstrates how to pass a custom EEPROM address and Wire.

  This library supports EEPROMs with any I2C address and
  any Wire hardware (Wire1, Wire2, etc) by passing them into begin.
  
  For this example the I2C EEPROM should have all its ADR0 to VCC, 
  ADR1 to GND, and ADR2 to GND.

  Hardware Connections:
  I used an Artemis for this example. Make sure to connect the PTH of Qwiic EEPROM to the pins of the seconday I2C bus.

  pin 0 on Artemis RedBoard = SDA on Qwiic EEPROM
  pin 6 = SCL
  GND to GND
  3.3V to 3.3V
  
  Load this sketch
  Open output window at 115200bps
*/

#include <Wire.h>

#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
ExternalEEPROM myMem;

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("I2C EEPROM example");
  
//  Wire1.setClock(400000); //set I2C communication to 400kHz
  Wire1.begin();

  #define EEPROM_ADDRESS 0b1010001 //0b1010(A2 A1 A0): A standard I2C EEPROM with the ADR0 bit set to VCC
    
  //Connect to a EEPROM with ADR0 set to VCC and use the Wire1 hardware to talk to the EEPROM
  if (myMem.begin(EEPROM_ADDRESS, Wire1) == false) //And Uno will fail to compile here
  {
    Serial.println("No memory detected. Freezing.");
    while (1);
  }
  Serial.println("Memory detected!");

  float myValue3 = -7.35;
  myMem.put(20, myValue3); //(location, data)
  float myRead3;
  myMem.get(20, myRead3); //location to read, thing to put data into
  Serial.print("I read: ");
  Serial.println(myRead3);  
}

void loop()
{

}
