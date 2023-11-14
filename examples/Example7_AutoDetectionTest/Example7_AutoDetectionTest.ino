/*
  Automatic size detection of EEPROM
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

  //randomSeed(analogRead(A0) * analogRead(A1));

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1)
      ;
  }
  Serial.println("Memory detected!");

  //  myMem.write(127, 25); //location, thing
  //  myMem.write(129, 36); //location, thing
  //
  //  uint8_t myVal1 = myMem.read(127);
  //  Serial.print("myVal1: ");
  //  Serial.println(myVal1);
  //
  //  uint8_t myVal2 = myMem.read(129);
  //  Serial.print("myVal2: ");
  //  Serial.println(myVal2);
  //
  //  while (1);

  //Write a series of bytes then test to see if auto-detect changes them
  //The detection functions *should not* modify the data on the EEPROM

  int maxBytes = 300; //Ideally we write this many bytes
  if (maxBytes > myMem.getMemorySizeBytes())
    maxBytes = myMem.getMemorySizeBytes();

  for (int x = 0 ; x < maxBytes ; x++)
    myMem.write(x, (uint8_t)('0' + x));

  //  Serial.print("Detected number of address bytes: ");
  //Serial.println(myMem.detectAddressBytes());

  Serial.print("Detected pageSizeBytes: ");
  Serial.println(myMem.detectPageSizeBytes());

  //  Serial.print("Detected page write time (ms): ");
  //  Serial.println(myMem.detectWriteTimeMs());

  //uint32_t eepromSizeBytes = myMem.detectMemorySizeBytes();
  uint32_t eepromSizeBytes = myMem.getMemorySizeBytes();
  Serial.print("Detected EEPROM size (bytes): ");
  Serial.print(eepromSizeBytes);
  Serial.print(" bytes / ");
  if (eepromSizeBytes < 128)
  {
    Serial.print(eepromSizeBytes * 8);
    Serial.print(" Bits");
  }
  else
  {
    Serial.print(eepromSizeBytes * 8 / 1024);
    Serial.print(" kBits");
  }
  Serial.print(" - 24XX");
  if (eepromSizeBytes == 16)
    Serial.print("00");
  else
  {
    if ((eepromSizeBytes * 8 / 1024) < 10) Serial.print("0");
    Serial.print(eepromSizeBytes * 8 / 1024);
  }
  Serial.println();

  Serial.print("Checking memory: ");
  bool allTestsPassed = true;
  for (int x = 0 ; x < maxBytes ; x++)
  {
    byte readValue = myMem.read(x);
    if (readValue != (uint8_t)('0' + x))
    {
      Serial.print("Error detected in location: ");
      Serial.print(x);
      allTestsPassed = false;
      break;
    }
  }

  if (allTestsPassed == true)
    Serial.print("Good");
  Serial.println();

  //Do basic read/write tests
  byte myValue1 = 200;
  myMem.write(0, myValue1); //(location, data)

  byte myRead1 = myMem.read(0);
  Serial.print("I read (should be 200): ");
  Serial.println(myRead1);

  int myValue2 = -366;
  myMem.put(10, myValue2); //(location, data)
  int myRead2;
  myMem.get(10, myRead2); //location to read, thing to put data into
  Serial.print("I read (should be -366): ");
  Serial.println(myRead2);

  if (myMem.getMemorySizeBytes() > 16)
  {
    float myValue3 = -7.35;
    myMem.put(20, myValue3); //(location, data)
    float myRead3;
    myMem.get(20, myRead3); //location to read, thing to put data into
    Serial.print("I read (should be -7.35): ");
    Serial.println(myRead3);

    String myString = "Hi, I am just a simple test string"; //34 characters
    unsigned long nextEEPROMLocation = myMem.putString(70, myString);

    String myRead4 = "";
    myMem.getString(70, myRead4);
    Serial.print("I read: ");
    Serial.println(myRead4);
    Serial.print("Next available EEPROM location: ");
    Serial.println(nextEEPROMLocation);
  }
}

void loop()
{
}