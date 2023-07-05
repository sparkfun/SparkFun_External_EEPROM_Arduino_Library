/*
  Set manual specs of an external I2C EEPROM
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 11th, 2019
  License: This code is public domain but you buy me a beer if you use this 
  and we meet someday (Beerware license).
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14764

  This example demonstrates how to set the various settings for a given EEPROM.
  Read the datasheet! Each EEPROM will have specific values for:
    Overall EEPROM size in bytes (512kbit = 65536, 256kbit = 32768).
    Bytes per page write (1, 8, 16, 32, 64, and 128 are common).
    Whether write polling is supported.
  Note: This will overwrite the auto-detected specs. It is recommended to use example 2
  to detect the device's settings then you can set them before 
  
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
  delay(10);
  Serial.println("I2C EEPROM example");

  Wire.begin();
  Wire.setClock(400000); //Most EEPROMs can run 400kHz and higher

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1);
  }
  Serial.println("Memory detected!");

  //Set settings for this EEPROM
  myMem.setMemorySizeBytes(512 * 1024 / 8); //In bytes. 512kbit = 64kbyte
  myMem.setPageSizeBytes(128); //In bytes. Has 128 byte page size.
  myMem.enablePollForWriteComplete(); //Supports I2C polling of write completion
  myMem.setPageWriteTimeMs(5); //5 ms max write time

  Serial.print("Mem size in bytes: ");
  Serial.println(myMem.length());

  float myValue3 = -7.35;
  myMem.put(20, myValue3); //(location, data)
  float myRead3;
  myMem.get(20, myRead3); //location to read, thing to put data into
  Serial.print("I read (should be -7.35): ");
  Serial.println(myRead3);
  
}

void loop()
{

}