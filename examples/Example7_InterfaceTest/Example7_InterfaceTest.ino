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
  delay(10);
  Serial.println("I2C EEPROM example");

  //Pick any unconnected analog pin
  randomSeed(analogRead(A0));
  //randomSeed(analogRead(A15));

  unsigned int randomLocation;

  Wire.begin();
  Wire.setClock(400000);
  //Wire.setClock(1000000);

  //Set the memory specs
  //Default to the Qwiic 24xx512 EEPROM: https://www.sparkfun.com/products/14764
  myMem.setMemoryType(512); // Valid types: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1)
      ;
  }
  Serial.println("Memory detected!");

  uint32_t eepromSizeBytes = myMem.getMemorySizeBytes();
  Serial.print("EEPROM type: 24xx");
  if (eepromSizeBytes == 16)
    Serial.print("00");
  else
  {
    if ((eepromSizeBytes * 8 / 1024) < 10) Serial.print("0");
    Serial.print(eepromSizeBytes * 8 / 1024);
  }
  Serial.print(" - bytes: ");
  Serial.print(eepromSizeBytes);
  Serial.println();

  Serial.print("Number of address bytes: ");
  Serial.println(myMem.getAddressBytes());
  
  Serial.print("Page size in bytes: ");
  Serial.println(myMem.getPageSizeBytes());
  
  bool allTestsPassed = true;

  //Erase test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.print("Time to erase all EEPROM: ");
  long startTime = millis();
  myMem.erase();
  long endTime = millis();
  Serial.print(endTime - startTime);
  Serial.println("ms");
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //Byte sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.println();
  Serial.println("8 bit tests");
  byte myValue1 = 200;
  byte myValue2 = 23;
  randomLocation = random(0, myMem.length() - (sizeof(byte) * 2));

  startTime = micros();
  myMem.write(randomLocation, myValue1); //(location, data)
  while (myMem.isConnected() == false); //Wait for write to complete
  endTime = micros();
  Serial.print("Time to record byte: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  myMem.put(randomLocation + sizeof(byte), myValue2);
  while (myMem.isConnected() == false); //Wait for write to complete

  startTime = micros();
  myMem.write(randomLocation, myValue1); //(location, data)
  while (myMem.isConnected() == false); //Wait for write to complete
  endTime = micros();
  Serial.print("Time to write identical byte to same location (should be ~0ms): ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  startTime = micros();
  byte response1 = myMem.read(randomLocation);
  endTime = micros();
  Serial.print("Time to read byte: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  byte response2 = myMem.read(randomLocation + sizeof(byte));
  Serial.print("Location ");
  Serial.print(randomLocation);
  Serial.print(" should be ");
  Serial.print(myValue1);
  Serial.print(": ");
  Serial.print(response1);
  if(myValue1 != response1)
    Serial.print(" - Fail");
  Serial.println();

  Serial.print("Location ");
  Serial.print(randomLocation + sizeof(byte));
  Serial.print(" should be ");
  Serial.print(myValue2);
  Serial.print(": ");
  Serial.print(response2);
  if(myValue2 != response2)
    Serial.print(" - Fail");
  Serial.println();

  if (myValue1 != response1)
    allTestsPassed = false;
  if (myValue2 != response2)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("16 bit tests");

  //int16_t and uint16_t tests
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  uint16_t myValue3 = 3411;
  int16_t myValue4 = -366;
  randomLocation = random(0, myMem.length() - (sizeof(int16_t) * 2));

  startTime = micros();
  myMem.put(randomLocation, myValue3);
  endTime = micros();
  Serial.print("Time to record int16: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  myMem.put(randomLocation + sizeof(int16_t), myValue4);

  uint16_t response3;
  int16_t response4;

  startTime = micros();
  myMem.get(randomLocation, response3);
  endTime = micros();
  Serial.print("Time to read int16: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  myMem.get(randomLocation + sizeof(int16_t), response4);
  Serial.print("Location ");
  Serial.print(randomLocation);
  Serial.print(" should be ");
  Serial.print(myValue3);
  Serial.print(": ");
  Serial.println(response3);

  Serial.print("Location ");
  Serial.print(randomLocation + sizeof(int16_t));
  Serial.print(" should be ");
  Serial.print(myValue4);
  Serial.print(": ");
  Serial.println(response4);

  if (myValue3 != response3)
    allTestsPassed = false;
  if (myValue4 != response4)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("int tests");

  //int and unsigned int tests
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.print("Size of int: ");
  Serial.println(sizeof(int)); //Uno reports this as 2
  int myValue5 = -2450;
  unsigned int myValue6 = 4001;
  randomLocation = random(0, myMem.length() - (sizeof(int) * 2));

  startTime = micros();
  myMem.put(randomLocation, myValue5);
  endTime = micros();
  Serial.print("Time to record int: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");
  myMem.put(randomLocation + sizeof(int), myValue6);

  int response5;
  unsigned int response6;

  startTime = micros();
  myMem.get(randomLocation, response5);
  endTime = micros();
  Serial.print("Time to read int: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  myMem.get(randomLocation + sizeof(int), response6);
  Serial.print("Location ");
  Serial.print(randomLocation);
  Serial.print(" should be ");
  Serial.print(myValue5);
  Serial.print(": ");
  Serial.println(response5);

  Serial.print("Location ");
  Serial.print(randomLocation + sizeof(int));
  Serial.print(" should be ");
  Serial.print(myValue6);
  Serial.print(": ");
  Serial.println(response6);
  if (myValue5 != response5)
    allTestsPassed = false;
  if (myValue6 != response6)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("32 bit tests");

  //int32_t and uint32_t sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  int32_t myValue7 = -341002;
  uint32_t myValue8 = 241544;
  randomLocation = random(0, myMem.length() - (sizeof(int32_t) * 2));

  myMem.put(randomLocation, myValue7);
  myMem.put(randomLocation + sizeof(int32_t), myValue8);

  int32_t response7;
  uint32_t response8;

  startTime = micros();
  myMem.get(randomLocation, response7);
  endTime = micros();
  Serial.print("Time to read int32: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  myMem.get(randomLocation + sizeof(int32_t), response8);
  Serial.print("Location ");
  Serial.print(randomLocation);
  Serial.print(" should be ");
  Serial.print(myValue7);
  Serial.print(": ");
  Serial.println(response7);

  Serial.print("Location ");
  Serial.print(randomLocation + sizeof(int32_t));
  Serial.print(" should be ");
  Serial.print(myValue8);
  Serial.print(": ");
  Serial.println(response8);
  if (myValue7 != response7)
    allTestsPassed = false;
  if (myValue8 != response8)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //float (32) sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.print("Size of float: ");
  Serial.println(sizeof(float));
  float myValue9 = -7.35;
  float myValue10 = 5.22;
  randomLocation = random(0, myMem.length() - (sizeof(float) * 2) );

  myMem.put(randomLocation, myValue9);
  myMem.put(randomLocation + sizeof(float), myValue10);

  float response9;
  float response10;

  startTime = micros();
  myMem.get(randomLocation, response9);
  endTime = micros();
  Serial.print("Time to read float: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  myMem.get(randomLocation + sizeof(float), response10);
  Serial.print("Location ");
  Serial.print(randomLocation);
  Serial.print(" should be ");
  Serial.print(myValue9);
  Serial.print(": ");
  Serial.println(response9);

  Serial.print("Location ");
  Serial.print(randomLocation + sizeof(float));
  Serial.print(" should be ");
  Serial.print(myValue10);
  Serial.print(": ");
  Serial.println(response10);

  if (myValue9 != response9)
    allTestsPassed = false;
  if (myValue10 != response10)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("64 bit tests");

  //double (64) sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.print("Size of double: ");
  Serial.println(sizeof(double));
  double myValue11 = -290.3485723409857;
  double myValue12 = 384.957; //34987;
  double myValue13 = 917.14159;
  double myValue14 = 254.8877;
  randomLocation = random(0, myMem.length() - (sizeof(double) * 2));

  startTime = micros();
  myMem.put(randomLocation, myValue11);
  endTime = micros();
  Serial.print("Time to record 64-bits: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  myMem.put(randomLocation + sizeof(double), myValue12);
  myMem.put(myMem.length() - sizeof(double), myValue13); //Test end of EEPROM space

  double response11;
  double response12;
  double response13;

  startTime = micros();
  myMem.get(randomLocation, response11);
  endTime = micros();
  Serial.print("Time to read 64-bits: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  myMem.get(randomLocation + sizeof(double), response12);
  myMem.get(myMem.length() - sizeof(double), response13);
  Serial.print("Location ");
  Serial.print(randomLocation);
  Serial.print(" should be ");
  Serial.print(myValue11);
  Serial.print(": ");
  Serial.println(response11);

  Serial.print("Location ");
  Serial.print(randomLocation + sizeof(double));
  Serial.print(" should be ");
  Serial.print(myValue12);
  Serial.print(": ");
  Serial.println(response12);

  Serial.print("Edge of EEPROM ");
  Serial.print(myMem.length() - sizeof(double));
  Serial.print(" should be ");
  Serial.print(myValue13);
  Serial.print(": ");
  Serial.println(response13);

  double response14;
  myMem.put(myMem.length() - sizeof(double), myValue14); //Test the re-write of a spot
  myMem.get(myMem.length() - sizeof(double), response14);
  Serial.print("Rewrite of ");
  Serial.print(myMem.length() - sizeof(double));
  Serial.print(" should be ");
  Serial.print(myValue14);
  Serial.print(": ");
  Serial.println(response14);

  if (myValue11 != response11)
    allTestsPassed = false;
  if (myValue12 != response12)
    allTestsPassed = false;
  if (myValue13 != response13)
    allTestsPassed = false;
  if (myValue14 != response14)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("Buffer Write Test");

  //Buffer write test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //char myChars[242] = "Lorem ipsum dolor sit amet, has in verterem accusamus. Nulla viderer inciderint eum at. Quo elit nullam malorum te, agam fuisset detracto an sea, eam ut liber aperiri. Id qui velit facilisi. Mel probatus definitionem id, eu amet vidisse eum.";
  char myChars[88] = "Lorem ipsum dolor sit amet, has in verterem accusamus. Nulla viderer inciderint eum at.";
  randomLocation = random(0, myMem.length() - sizeof(myChars));

  Serial.print("Calculated time to record array of ");
  Serial.print(sizeof(myChars));
  Serial.print(" characters: ~");
  Serial.print((uint32_t)sizeof(myChars) / myMem.getPageSizeBytes() * myMem.getWriteTimeMs());
  Serial.println("ms");

  startTime = micros();
  myMem.put(randomLocation, myChars);
  endTime = micros();
  Serial.print("Time to record array: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  char readMy[sizeof(myChars)];

  startTime = micros();
  myMem.get(randomLocation, readMy);
  endTime = micros();
  Serial.print("Time to read array: ");
  Serial.print(endTime - startTime);
  Serial.println(" us");

  //  Serial.print("Location ");
  //  Serial.print(randomLocation);
  //  Serial.print(" string should read:");
  //  Serial.println(myChars);
  //  Serial.println(readMy);
  if (strcmp(myChars, readMy) != 0)
  {
    Serial.println("String compare failed");
    allTestsPassed = false;
  }

  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println();
  Serial.print("Memory Contents:");
  for (uint16_t x = 0; x < 32 * 4; x++)
  {
    if (x % 16 == 0)
      Serial.println();
    Serial.print(" 0x");
    if (myMem.read(x) < 0x10)
      Serial.print("0");
    Serial.print(myMem.read(x), HEX);
  }
  Serial.println();

  if (allTestsPassed == true)
    Serial.println("All tests PASSED!");
  else
    Serial.println("Something went wrong. See output.");
}

void loop()
{
}