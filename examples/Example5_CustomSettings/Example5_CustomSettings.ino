/*
  Explicitly tell the library the size of the EEPROM, page size, and address bytes
  Author: Nathan Seidle
  Created: June 23, 2023
  License: Lemonadeware. Buy me a lemonade (or other) someday.

  This sketch demonstrates how to detect memory size, address bytes,
  page size, and write time. You can also pre-assign these specs to avoid
  the start up delay caused by the detection routines.

  Known/compatible memory types (basically all I2C EEPROMs):
   24xx00 - 128 bit / 16 bytes - 1 address byte, 1 byte page size
   24xx01 - 1024 bit / 128 bytes - 1 address byte, 8 byte page size
   24xx02 - 2048 bit / 256 bytes - 1 address byte, 8 byte page size
   24xx04 - 4096 bit / 512 bytes - 1 address byte, 16 byte page size
   24xx08 - 8192 bit / 1024 bytes - 1 address byte, 16 byte page size
   24xx16 - 16384 bit / 2048 bytes - 1 address byte, 16 byte page size
   24xx32 - 32768 bit / 4096 bytes - 2 address bytes, 32 byte page size
   24xx64 - 65536 bit / 8192 bytes - 2 address bytes, 32 byte page size
   24xx128 - 131072 bit / 16384 bytes - 2 address bytes, 64 byte page size
   24xx256 - 262144 bit / 32768 bytes - 2 address bytes, 64 byte page size
   24xx512 - 524288 bit / 65536 bytes - 2 address bytes, 128 byte page size
   24xx1025 - 1024000 bit / 128000 byte - 2 address bytes, 128 byte page size
   24xxM02 - 2097152 bit / 262144 byte - 2 address bytes, 256 byte page size
*/

#include <Wire.h>

#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
ExternalEEPROM myMem;

void setup()
{
  Serial.begin(115200);
  Serial.println("Qwiic EEPROM example");

  Wire.begin();

  //Explicitly set the address bytes, page size, and memory size for this EEPROM
  // 24xx512 - 524288 bit / 65536 bytes - 2 address bytes, 128 byte page
  myMem.setMemorySizeBytes(65536); //This function will set the AddressBytes and PageSize.
  myMem.setAddressBytes(2); //Call these functions after MemorySizeBytes. Only needed if you have a very unique EEPROM with odd Address Bytes and Page Sizes.
  myMem.setPageSizeBytes(128);

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