/*
  Read and write settings and calibration data to an external I2C EEPROM
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 11th, 2019
  License: This code is public domain but you buy me a beer if you use this 
  and we meet someday (Beerware license).
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14764

  This example demonstrates how to read and write various variables to memory.

  The I2C EEPROM should have all its ADR pins set to GND (0). This is default
  on the Qwiic board.

  Hardware Connections:
  Plug the SparkFun Qwiic EEPROM to an Uno, Artemis, or other Qwiic equipped board
  Load this sketch
  Open output window at 115200bps
*/

#include <Wire.h>

#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
ExternalEEPROM myMem;

void setup()
{
  Serial.begin(115200);
  Serial.println("Qwiic EEPROM example");

  Wire.begin();

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1)
      ;
  }
  Serial.println("Memory detected!");

  Serial.print("Mem size in bytes: ");
  Serial.println(myMem.length());

  //Yes you can read and write bytes, but you shouldn't!
  byte myValue1 = 200;
  myMem.write(0, myValue1); //(location, data)

  byte myRead1 = myMem.read(0);
  Serial.print("I read: ");
  Serial.println(myRead1);

  //You should use gets and puts. This will automatically and correctly arrange
  //the bytes for larger variable types.
  int myValue2 = -366;
  myMem.put(10, myValue2); //(location, data)
  int myRead2;
  myMem.get(10, myRead2); //location to read, thing to put data into
  Serial.print("I read: ");
  Serial.println(myRead2);

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
