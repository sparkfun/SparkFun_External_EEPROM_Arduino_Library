/*
  Automatic size detection of EEPROM
  Author: Nathan Seidle
  Created: June 23, 2023
  License: Lemonadeware. Buy me a lemonade (or other) someday.

  This sketch demonstrates how to auto-detect the size of an EEPROM.

  Note: This will overwrite data! It attempts to write to locations that may contain 
  data so use at your own risk.
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

  //Be careful! detectMemorySizeBytes() will write data do the EEPROM, potentially overwriting valuable data
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