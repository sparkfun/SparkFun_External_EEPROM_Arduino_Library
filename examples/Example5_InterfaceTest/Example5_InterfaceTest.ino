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

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1)
      ;
  }
  Serial.println("Memory detected!");

  myMem.setMemorySize(512000 / 8); //Qwiic EEPROM is the 24512C (512k bit)
  //myMem.setPageSize(128);
  //myMem.disablePollForWriteComplete();

  bool allTestsPassed = true;

  Serial.print("Mem size in bytes: ");
  Serial.println(myMem.length());

  //Erase test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  long startTime = millis();
  myMem.erase();
  long endTime = millis();
  Serial.print("Time to erase all EEPROM: ");
  Serial.print(endTime - startTime);
  Serial.println("ms");
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //Byte sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.println("");
  Serial.println("8 bit tests");
  byte myValue1 = 200;
  byte myValue2 = 23;
  randomLocation = random(0, myMem.length());

  startTime = micros();
  myMem.write(randomLocation, myValue1); //(location, data)
  endTime = micros();
  myMem.put(randomLocation + 1, myValue2);
  Serial.println("Time to record byte: " + (String)(endTime - startTime) + " us");

  startTime = micros();
  myMem.write(randomLocation, myValue1); //(location, data)
  endTime = micros();
  Serial.println("Time to write identical byte to same location (should be ~0ms): " + (String)(endTime - startTime) + " us");

  startTime = micros();
  byte response1 = myMem.read(randomLocation);
  endTime = micros();
  Serial.println("Time to read byte: " + (String)(endTime - startTime) + " us");

  byte response2 = myMem.read(randomLocation + 1);
  Serial.println("Location " + (String)randomLocation + " should be " + (String)myValue1 + ": " + (String)response1);
  Serial.println("Location " + (String)(randomLocation + 1) + " should be " + (String)myValue2 + ": " + (String)response2);
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
  randomLocation = random(0, myMem.length());

  startTime = micros();
  myMem.put(randomLocation, myValue3);
  endTime = micros();
  Serial.println("Time to record int16: " + (String)(endTime - startTime) + " us");

  myMem.put(randomLocation + 2, myValue4);

  uint16_t response3;
  int16_t response4;

  startTime = micros();
  myMem.get(randomLocation, response3);
  endTime = micros();
  Serial.println("Time to read int16: " + (String)(endTime - startTime) + " us");

  myMem.get(randomLocation + 2, response4);
  Serial.println("Location " + (String)randomLocation + " should be " + (String)myValue3 + ": " + (String)response3);
  Serial.println("Location " + (String)(randomLocation + 2) + " should be " + (String)myValue4 + ": " + (String)response4);
  if (myValue3 != response3)
    allTestsPassed = false;
  if (myValue4 != response4)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("32 bit tests");

  //int and unsigned int (32) tests
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.println("Size of int: " + (String)sizeof(int));
  int myValue5 = -245000;
  unsigned int myValue6 = 400123;
  randomLocation = random(0, myMem.length());

  startTime = micros();
  myMem.put(randomLocation, myValue5);
  endTime = micros();
  Serial.println("Time to record int32: " + (String)(endTime - startTime) + " us");
  myMem.put(randomLocation + 4, myValue6);

  int response5;
  unsigned int response6;

  startTime = micros();
  myMem.get(randomLocation, response5);
  endTime = micros();
  Serial.println("Time to read int32: " + (String)(endTime - startTime) + " us");

  myMem.get(randomLocation + 4, response6);
  Serial.println("Location " + (String)randomLocation + " should be " + (String)myValue5 + ": " + (String)response5);
  Serial.println("Location " + (String)(randomLocation + 4) + " should be " + (String)myValue6 + ": " + (String)response6);
  if (myValue5 != response5)
    allTestsPassed = false;
  if (myValue6 != response6)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //int32_t and uint32_t sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  int32_t myValue7 = -341002;
  uint32_t myValue8 = 241544;
  randomLocation = random(0, myMem.length());

  myMem.put(randomLocation, myValue7);
  myMem.put(randomLocation + 4, myValue8);

  int32_t response7;
  uint32_t response8;

  startTime = micros();
  myMem.get(randomLocation, response7);
  endTime = micros();
  Serial.println("Time to read int32: " + (String)(endTime - startTime) + " us");

  myMem.get(randomLocation + 4, response8);
  Serial.println("Location " + (String)randomLocation + " should be " + (String)myValue7 + ": " + (String)response7);
  Serial.println("Location " + (String)(randomLocation + 4) + " should be " + (String)myValue8 + ": " + (String)response8);
  if (myValue7 != response7)
    allTestsPassed = false;
  if (myValue8 != response8)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //float (32) sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.println("Size of float: " + (String)sizeof(float));
  float myValue9 = -7.35;
  float myValue10 = 5.22;
  randomLocation = random(0, myMem.length());

  myMem.put(randomLocation, myValue9);
  myMem.put(randomLocation + 4, myValue10);

  float response9;
  float response10;

  startTime = micros();
  myMem.get(randomLocation, response9);
  endTime = micros();
  Serial.println("Time to read float: " + (String)(endTime - startTime) + " us");

  myMem.get(randomLocation + 4, response10);
  Serial.println("Location " + (String)randomLocation + " should be " + (String)myValue9 + ": " + (String)response9);
  Serial.println("Location " + (String)(randomLocation + 4) + " should be " + (String)myValue10 + ": " + (String)response10);
  if (myValue9 != response9)
    allTestsPassed = false;
  if (myValue10 != response10)
    allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("64 bit tests");

  //double (64) sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.println("Size of double: " + (String)sizeof(double));
  double myValue11 = -290.3485723409857;
  double myValue12 = 384.957; //34987;
  double myValue13 = 917.14159;
  double myValue14 = 254.8877;
  randomLocation = random(0, myMem.length());

  startTime = micros();
  myMem.put(randomLocation, myValue11);
  endTime = micros();
  Serial.println("Time to record 64-bits: " + (String)(endTime - startTime) + " us");

  myMem.put(randomLocation + 8, myValue12);
  myMem.put(myMem.length() - sizeof(myValue13), myValue13); //Test end of EEPROM space

  double response11;
  double response12;
  double response13;

  startTime = micros();
  myMem.get(randomLocation, response11);
  endTime = micros();
  Serial.println("Time to read 64-bits: " + (String)(endTime - startTime) + " us");

  myMem.get(randomLocation + 8, response12);
  myMem.get(myMem.length() - sizeof(myValue13), response13);
  Serial.println("Location " + (String)randomLocation + " should be " + (String)myValue11 + ": " + (String)response11);
  Serial.println("Location " + (String)(randomLocation + 8) + " should be " + (String)myValue12 + ": " + (String)response12);
  Serial.println("Edge of EEPROM " + (String)(myMem.length() - sizeof(myValue13)) + " should be " + (String)myValue13 + ": " + (String)response13);

  double response14;
  myMem.put(myMem.length() - sizeof(myValue14), myValue14); //Test the re-write of a spot
  myMem.get(myMem.length() - sizeof(myValue14), response14);
  Serial.print("Rewrite of ");
  Serial.print(myMem.length() - sizeof(myValue14));
  Serial.println(" should be " + (String)myValue14 + ": " + (String)response14);
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
  randomLocation = random(0, myMem.length());

  Serial.print("Calculated time to record array of " + (String)sizeof(myChars) + " characters: ~");
  Serial.print((uint32_t)sizeof(myChars) / myMem.getPageSize() * myMem.getPageWriteTime());
  Serial.println("ms");

  startTime = micros();
  myMem.put(randomLocation, myChars);
  endTime = micros();
  Serial.println("Time to record array: " + (String)(endTime - startTime) + " us");

  char readMy[sizeof(myChars)];

  startTime = micros();
  myMem.get(randomLocation, readMy);
  endTime = micros();
  Serial.println("Time to read array: " + (String)(endTime - startTime) + " us");

  Serial.println("Location " + (String)randomLocation + " string should read:");
  Serial.println(myChars);
  Serial.println(readMy);
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
