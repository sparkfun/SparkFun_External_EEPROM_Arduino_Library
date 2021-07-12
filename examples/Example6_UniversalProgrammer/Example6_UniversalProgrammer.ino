/*
  "Universal" EEPROM programmer for Arduino
  Author: Nathan Seidle
  Created: March 25, 2020
  License: Lemonadeware. Buy me a lemonade (or other) someday.

  This sketch demonstrates how to create a I2C programmer that reads
  any binary data file from the SD card and writes it as quickly as possible to the EEPROM.
  User can verify a file against an EEPROM. Autoprogram mode allows a user to batch program
  a large number of EEPROMs.

  This sketch was created because the software currently available for the CH341 is terrible and often
  lacks newly released EEPROMs such as the 24LC1025. This library and sketch demonstrate
  how to throw out the windows program and use hardware instead.

  Uno is supported but is *very* slow. Artemis is ~10x faster (https://www.sparkfun.com/products/15444).

  Wire a microSD breakout to the SPI pins:

  SCK on Artemis RedBoard = SCK on microSD breakout (https://www.sparkfun.com/products/544)
  MISO = DO
  MOSI = DI
  10 = CS
  GND = GND
  3.3V = VCC

  Connect Qwiic EEPROM with Qwiic cable

  To really test the full functionality of this example, make sure to load an SD card with a file
  named data.bin and some made up binary data.

  Note: On larger EEPROMs, pin 3 must be tied to 3.3V. RTFM.

  Current benchmarking:
  112586 bytes takes 3.801s to write or 29.6kB/s

  TODO:
  Modify all library features config from menu
  View files and select a file
*/

#include <Wire.h>

const byte PIN_STAT_LED = 13;

//microSD Interface
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include <SPI.h>
#include <SD.h>
const byte PIN_MICROSD_CHIP_SELECT = 10;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
ExternalEEPROM myMem;

const byte EEPROM_ADDRESS = 0b1010000; //The 7-bit unshifted address for the 24LC1025
String binFileName = "data.bin";       //File to write to the EEPROM

void setup()
{
  Serial.begin(115200);
  Serial.println(F("SparkFun I2C EEPROM Writer"));

  pinMode(PIN_STAT_LED, OUTPUT);

  Wire.begin();
  Wire.setClock(400000);

  SPI.begin();

  beginSD();

  //Set settings for a 24LC1025
  myMem.setMemorySize(1024000 / 8); //In bytes. 1024kbit = 64kbyte
  myMem.setPageSize(128);           //In bytes.
  myMem.enablePollForWriteComplete();
}

void loop()
{
  while (myMem.begin(EEPROM_ADDRESS) == false)
  {
    Serial.println(F("No memory detected."));
    delay(250);
  }

  Serial.println();
  Serial.println(F("Memory detected!"));
  Serial.print(F("Current file to record: "));
  Serial.println(binFileName);
  Serial.println(F("w)rite file to EEPROM"));
  Serial.println(F("v)erify EEPROM against file"));
  Serial.println(F("s)et file name to read from"));
  Serial.println(F("e)rase EEPROM"));
  Serial.println(F("a)uto program"));
  Serial.println();

  while (Serial.available())
    Serial.read();
  while (Serial.available() == 0)
    ;
  byte choice = Serial.read();

  if (choice == 'w')
  {
    Serial.println(F("Writing file to EEPROM"));
    writeFileToEEPROM((char *)"data.bin");
  }
  else if (choice == 'v')
  {
    Serial.println(F("Verifying"));
    verifyFileOnEEPROM((char *)"data.bin");
  }
  else if (choice == 's')
  {
    Serial.print(F("Enter file to open: "));
    binFileName = Serial.readString();
  }
  else if (choice == 'e')
  {
    Serial.println(F("This will take a moment. Be patient."));
    myMem.erase();
    Serial.println(F("Erase complete"));
  }
  else if (choice == 'a')
  {
    autoProgram();
  }
  else
  {
    Serial.print(F("Unknown: "));
    Serial.write(choice);
    Serial.println();
  }
}

void autoProgram()
{
  int programCount = 0;

  delay(50); //Wait for any incoming characters to be received

  //Clear the serial buffer
  while (Serial.available())
    Serial.read();

  Serial.println(F("Auto Programming!"));
  Serial.println(F("Press any key to exit"));

  while (1)
  {
    if (Serial.available())
      return;

    if (myMem.begin(EEPROM_ADDRESS) == true)
    {
      Serial.println(F("New device detected"));

      delay(500); //Wait for user to get IC inserted into breadboard

      if (writeFileToEEPROM((char *)"data.bin") == false)
      {
        Serial.println(F("Write failed"));
        continue;
      }

      if (verifyFileOnEEPROM((char *)"data.bin") == false)
      {
        Serial.println(F("Verify failed"));
        continue;
      }

      programCount++;

      Serial.print(F("Program count: "));
      Serial.println(programCount);

      //Wait for user to remove this device
      Serial.println(F("Remove IC and insert new one"));
      while (myMem.begin(EEPROM_ADDRESS) == true)
        delay(1000);
    }
    else
    {
      Serial.print(F("No IC detected"));
      while (myMem.begin(EEPROM_ADDRESS) == false)
      {
        delay(1000);
        Serial.print(F("."));
      }
      Serial.println();
    }
  }
}

//Given a file name, verify the contents of the EEPROM match the file contents
bool verifyFileOnEEPROM(char *fileName)
{
  File binFile = SD.open(fileName);
  if (binFile == false)
  {
    Serial.print(F("Programming file '"));
    Serial.print((String)fileName);
    Serial.println(F("' not found"));
    return (false);
  }
  Serial.println(F("Verification file opened"));

  int error = 0;
  uint32_t EEPROMLocation = 0;
  uint8_t *onEEPROM = (uint8_t *)malloc(myMem.getPageSize()); //Create a buffer to hold a page

  while (binFile.available())
  {
    uint8_t bytesToRead = myMem.getPageSize();

    if (EEPROMLocation + bytesToRead > myMem.getMemorySize())
      bytesToRead = myMem.getMemorySize() - EEPROMLocation;

    if (bytesToRead > binFile.available())
      bytesToRead = binFile.available();

    //myMem.get(EEPROMLocation, onEEPROM); //Location, data
    myMem.read(EEPROMLocation, onEEPROM, myMem.getPageSize()); //Location, data

    //Verify what was read from the EEPROM matches the file
    for (int x = 0; x < bytesToRead; x++)
    {
      uint8_t onFile = binFile.read();

      if (onEEPROM[x] != onFile)
      {
        Serial.print(F("Verify failed at location 0x"));
        Serial.print(EEPROMLocation + x, HEX);
        Serial.print(F(". Read 0x"));
        Serial.print(onEEPROM[x], HEX);
        Serial.print(F(", expected 0x"));
        Serial.print(onFile, HEX);
        Serial.println(F("."));
        if (error++ > 4)
        {
          binFile.close();
          return (false);
        }
      }
    }

    EEPROMLocation += bytesToRead;

    if (EEPROMLocation % (myMem.getPageSize() * 128) == 0)
      Serial.print(F("."));

    if (digitalRead(PIN_STAT_LED) == LOW)
      digitalWrite(PIN_STAT_LED, HIGH);
    else
      digitalWrite(PIN_STAT_LED, LOW);
  }

  Serial.println(F("Verification PASSED!"));

  binFile.close();
  free(onEEPROM);
  return (true);
}

bool writeFileToEEPROM(char *fileName)
{
  File binFile = SD.open(fileName);
  if (binFile == false)
  {
    Serial.print(F("Programming file '"));
    Serial.print((String)fileName);
    Serial.println(F("' not found"));
    return (false);
  }
  Serial.print(F("Programming file opened: "));
  Serial.print(binFile.size());
  Serial.println(F(" bytes"));

  //The estimated write time is composed of many things:
  //Number of bytes we need to write
  //The amount of time it takes to transfer a byteLimit: 30 bytes on Uno, overwrite with setI2CBufferSize (Wire clock speed / 10 / number of bytes)
  //The amount of time it takes to record a page (setPageWrite time = 5ms)
  //The number of writes we have to do (totalBytes / bytelimit)
  float timePerDataTransfer = 1.0 / (400000.0 / 10.0) * myMem.getI2CBufferSize();
  float timePerPageWrite = 0.005;
  int writesRequired = binFile.size() / myMem.getI2CBufferSize();

  //Serial.print(F("timePerByteLimit: "));
  //Serial.println(timePerByteLimit, 6);

  //Serial.print(F("writesRequired: "));
  //Serial.println(writesRequired);

  Serial.print(F("Estimated write time: "));
  Serial.print((timePerDataTransfer + timePerPageWrite) * writesRequired, 3);
  Serial.println(F("s"));

  uint8_t *buffer = (uint8_t *)malloc(myMem.getPageSize()); //Create a buffer to hold a page
  uint8_t bufferLocation = 0;

  uint32_t EEPROMLocation = 0;

  long startTime = millis();
  while (binFile.available())
  {
    buffer[bufferLocation++] = binFile.read();

    //If the buffer if full, record to EEPROM
    if (bufferLocation == myMem.getPageSize())
    {
      bufferLocation = 0;

      //Record buffer to EEPROM
      myMem.write(EEPROMLocation, buffer, myMem.getPageSize()); //Location, data

      EEPROMLocation += myMem.getPageSize();

      if (EEPROMLocation % (myMem.getPageSize() * 128) == 0)
        Serial.print(F("."));

      if (digitalRead(PIN_STAT_LED) == LOW)
        digitalWrite(PIN_STAT_LED, HIGH);
      else
        digitalWrite(PIN_STAT_LED, LOW);
    }
  }

  //Write anything remaining in the buffer
  if (bufferLocation > 0)
    myMem.write(EEPROMLocation, buffer, bufferLocation); //Location, data, length

  long endTime = millis();

  Serial.println();
  Serial.print(F("Write complete. Elapsed time: "));
  Serial.print((endTime - startTime) / 1000.0, 3);
  Serial.println(F("s"));

  binFile.close();
  free(buffer);
  return (true);
}

void beginSD()
{
  pinMode(PIN_MICROSD_CHIP_SELECT, OUTPUT);
  digitalWrite(PIN_MICROSD_CHIP_SELECT, HIGH); //Be sure SD is deselected

  //Max power up time is 250ms: https://www.kingston.com/datasheets/SDCIT-specsheet-64gb_en.pdf
  delay(10);

  if (SD.begin(PIN_MICROSD_CHIP_SELECT) == false)
  {
    delay(250); //Give SD more time to power up, then try again
    if (SD.begin(PIN_MICROSD_CHIP_SELECT) == false)
    {
      Serial.println("SD init failed. Do you have the correct board selected in Arduino? Is card present? Formatted?");
      digitalWrite(PIN_MICROSD_CHIP_SELECT, HIGH); //Be sure SD is deselected
      return;
    }
  }

  Serial.println("SD card online");
}
