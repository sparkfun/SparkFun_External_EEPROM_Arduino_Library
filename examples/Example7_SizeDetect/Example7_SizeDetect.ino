/*
  Automatic size detection of EEPROM
  Author: Nathan Seidle
  Created: June 23, 2023
  License: Lemonadeware. Buy me a lemonade (or other) someday.

  This sketch demonstrates how to auto-detect the size of an EEPROM.

  This sketch demonstrates how to automatically detect the size of an EEPROM.

  For smaller EEPROMs, 128, 1024, and to 2048 bit, one address byte is needed
  and three I2C address bits are available.
  24LC00 - 128 bit / 16 bytes
  24LC01 - 1024 bit / 128 bytes
  24LC02 - 2048 bit / 256 bytes

  For EEPROMs of 4k, 8k, and 16k bit, there are three bits called
  'block select bits' inside the address byte that are used
  24LC04 - 4096 bit / 512 bytes
  24LC08 - 8192 bit / 1024 bytes
  24LC16 - 16384 bit / 2048 bytes

  For 32k, 64k, 128k, 256k, and 512k bit we need two address bytes
  24LC32 - 32768 bit / 4096 bytes
  24LC64 - 65536 bit / 8192 bytes
  24LC128 - 131072 bit / 16384 bytes
  24LC256 - 262144 bit / 32768 bytes
  24LC512 - 524288 bit / 65536 bytes

  At 1Mbit (128,000 byte) there are two address bytes and a block select bit is used
  but at the upper end of the address bits (so instead of A2/A1/A0 it's B0/A1/A0).
  24LC1024 - 1024000 bit / 128000 byte
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

  uint32_t eepromSizeBytes = myMem.detectMemorySizeBytes();
  Serial.print("Detected size (in bytes): ");
  Serial.print(eepromSizeBytes);
  if (eepromSizeBytes < 128)
  {
    Serial.print(" / (in bits): ");
    Serial.print(eepromSizeBytes * 8);
  }
  else
  {
    Serial.print(" / (in kBits): ");
    Serial.print(eepromSizeBytes * 8 / 1024);
  }
  Serial.println();
}

void loop()
{
}