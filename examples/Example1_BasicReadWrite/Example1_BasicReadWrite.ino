/*
  Read and write settings and calibration data to an external I2C EEPROM
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 11th, 2019
  License: This code is public domain but you buy me a beer if you use this
  and we meet someday (Beerware license).
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/18355

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
  //delay(250); //Often needed for ESP based platforms
  Serial.println("Qwiic EEPROM example");

  Wire.begin();

  //We must set the memory specs. Pick your EEPROM From the list:

  // 24xx00 - 128 bit / 16 bytes - 1 address byte, 1 byte page
  // 24xx01 - 1024 bit / 128 bytes - 1 address byte, 8 byte page
  // 24xx02 - 2048 bit / 256 bytes - 1 address byte, 8 byte page
  // 24xx04 - 4096 bit / 512 bytes - 1 address byte, 16 byte page
  // 24xx08 - 8192 bit / 1024 bytes - 1 address byte, 16 byte page
  // 24xx16 - 16384 bit / 2048 bytes - 1 address byte, 16 byte page
  // 24xx32 - 32768 bit / 4096 bytes - 2 address bytes, 32 byte page
  // 24xx64 - 65536 bit / 8192 bytes - 2 address bytes, 32 byte page
  // 24xx128 - 131072 bit / 16384 bytes - 2 address bytes, 64 byte page
  // 24xx256 - 262144 bit / 32768 bytes - 2 address bytes, 64 byte page
  // 24xx512 - 524288 bit / 65536 bytes - 2 address bytes, 128 byte page
  // 24xx1025 - 1024000 bit / 128000 byte - 2 address byte, 128 byte page
  // 24xxM02 - 2097152 bit / 262144 byte - 2 address bytes, 256 byte page

  // Setting the memory type configures the memory size in bytes, the number of address bytes, and the page size in bytes.

  // Default to the Qwiic 24xx512 EEPROM: https://www.sparkfun.com/products/18355
  myMem.setMemoryType(512); // Valid types: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1025, 2048

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (true)
      ;
  }
  Serial.println("Memory detected!");

  Serial.print("Mem size in bytes: ");
  Serial.println(myMem.length());

  //Yes you can read and write bytes, but you shouldn't!
  byte myValue1 = 200;
  myMem.write(0, myValue1); //(location, data)

  byte myRead1 = myMem.read(0);
  Serial.print("I read (should be 200): ");
  Serial.println(myRead1);

  //You should use gets and puts. This will automatically and correctly arrange
  //the bytes for larger variable types.
  int myValue2 = -366;
  myMem.put(10, myValue2); //(location, data)
  int myRead2;
  myMem.get(10, myRead2); //location to read, thing to put data into
  Serial.print("I read (should be -366): ");
  Serial.println(myRead2);

  float myValue3 = -7.35;
  myMem.put(20, myValue3); //(location, data)
  float myRead3;
  myMem.get(20, myRead3); //location to read, thing to put data into
  Serial.print("I read (should be -7.35): ");
  Serial.println(myRead3);

  String myString = "Hi, I am just a simple test string";
  unsigned long nextEEPROMLocation = myMem.putString(30, myString);
  String myRead4 = "";
  myMem.getString(30, myRead4);
  Serial.print("I read: ");
  Serial.println(myRead4);
  Serial.print("Next available EEPROM location: ");
  Serial.println(nextEEPROMLocation);
}

void loop()
{
}