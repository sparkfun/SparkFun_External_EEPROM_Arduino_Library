/*
  Automatic size detection of EEPROM
  Author: Nathan Seidle
  Created: June 23, 2023
  License: Lemonadeware. Buy me a lemonade (or other) someday.

  This sketch demonstrates how to detect memory size, address bytes, 
  page size, and write time. You can also pre-assign these specs to avoid
  the start up delay caused by the detection routines.

  Known/compatible memory types (basically all I2C EEPROMs):
   24XX00 - 128 bit / 16 bytes - 1 address byte, 1 byte page size
   24XX01 - 1024 bit / 128 bytes - 1 address byte, 8 byte page size
   24XX02 - 2048 bit / 256 bytes - 1 address byte,
   24XX04 - 4096 bit / 512 bytes - 1 address byte, 16 byte page size
   24XX08 - 8192 bit / 1024 bytes - 1 address byte,
   24XX16 - 16384 bit / 2048 bytes - 1 address byte, 16 byte page size
   24XX32 - 32768 bit / 4096 bytes - 2 address bytes, 32 byte page size
   24XX64 - 65536 bit / 8192 bytes - 2 address bytes, 32 byte page size
   24XX128 - 131072 bit / 16384 bytes - 2 address bytes,
   24XX256 - 262144 bit / 32768 bytes - 2 address bytes,
   24XX512 - 524288 bit / 65536 bytes - 2 address bytes, 128 byte page size
   24XX1024 - 1024000 bit / 128000 byte - 2 address bytes, 128 byte page size
   24XXM02 - 2097152 bit / 262144 byte - 2 address bytes, 256 byte page size
*/

#include <Wire.h>

#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
ExternalEEPROM myMem;

void setup()
{
  Serial.begin(115200);
  Serial.println("Qwiic EEPROM example");

  Wire.begin();

  //The memory specs can be set before begin() to skip the auto-detection delay and write wear
  //24LC16 - 16384 bit / 2048 bytes - 1 address byte, 16 byte page size
  //myMem.setAddressBytes(1);
  //myMem.setPageSizeBytes(16);
  //myMem.setMemorySizeBytes(2048);

  //If specs are not available at start, they are auto-detected
  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1)
      ;
  }
  Serial.println("Memory detected!");

   Serial.print("Detected number of address bytes: ");
  Serial.println(myMem.detectAddressBytes());

  Serial.print("Detected pageSizeBytes: ");
  Serial.println(myMem.detectPageSizeBytes());

  Serial.print("Detected page write time (ms): ");
  Serial.println(myMem.detectWriteTimeMs());

  uint32_t eepromSizeBytes = myMem.detectMemorySizeBytes();
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
}

void loop()
{
}