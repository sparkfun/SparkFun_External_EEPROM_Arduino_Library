/*
  Automatic size detection of EEPROM
  Author: Nathan Seidle
  Created: June 23, 2023
  License: Lemonadeware. Buy me a lemonade (or other) someday.

  This sketch demonstrates how to detect memory size, address bytes,
  page size, and write time.

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

  //We don't need to specify the memory specs before begin(). We're just looking
  //for a I2C device to ACK.

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1)
      ;
  }
  Serial.println("Memory detected!");

  Serial.print("Detected number of address bytes: ");
  Serial.println(myMem.detectAddressBytes());

  //Page size detection is limited by the platform. For example, the Uno has an I2C buffer
  //that is 32 bytes. Therefore, page sizes above 16 bytes cannot be detected or used. For maximum
  //write speeds to an EEPROM, use a platform with a large I2C buffer (ie ESP32 is 128 bytes)
  //and an EEPROM with a large page size (24XX512 is 128 bytes).
  Serial.print("Detected pageSizeBytes: ");
  Serial.println(myMem.detectPageSizeBytes());

  //The EEPROM write time is 5 ms for all EEPROMs currently manufactured. 
  //Automatically detecting the write time is therefore not generally needed,
  //but it's here if needed.
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