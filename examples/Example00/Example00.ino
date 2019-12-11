/*
  Data for each memory

  Part Number: 24LC256 (24 = I2C, LC = 2.5-5.5V (AA=1.7-5.5@400kHz, FC=1.7-5.5@1MHz), 256 = 256kbit)
  Interface type: I2C
  I2C address: 0b1010000
  CS pin: N/A
  Size in bits: 256kbit, 1028kbit, etc
  Size of page writes in bytes: 64
  Max interface speed: 400kHz (1MHz, etc)
  Polling allowed: Yes
  Page write time (max, in ms): 5ms (not needed if we are polling)

  Part Number: 25Q32FVSIG (25 = SPI, Q = 2.7 to 3.6V?, 32 = 32Mbit, 104MHz/120MHz max SPI
  Block erase time can vary from 120ms to 1600ms, chip from 10s to 50s (wow)
  Interface type: SPI
  I2C address: N/A
  CS pin: 
  SPI Freq:
  Size in bits: 256kbit, 1028kbit, etc
  Size of page writes in bytes: 256 bytes per page
  Max interface speed: 400kHz (1MHz, etc)
  Polling allowed: Yes
  Page write time (max, in ms): 5ms (not needed if we are polling)

  TODO:
  (yes, done) Can you write more than 32 bytes at a time? Or is it limited to 32 bytes per I2C write transaction?
  (good) Does the bulk read go fast enough that its working or is it using single byte reads?
  (not needed?) Find way to bring settings into cpp so user can myMem.settings.pageSize.
  (yes, done) Can we poll the IC to see if its done writing?
  Pass "24LC256" into .begin() to get settings?
  (done) Get isBusy into ::
  I2C:
  Write a 'detect eeprom size' function? We could do a read, then attempt to write, then re-write orig if successful
  SPI:
  get unique 64 bit ID
  Get discoverable parameters register
  scan area we are about to write to. If blank, write. If not blank, read sector into local temp array
  hmm, won't work on Uno 
  
*/

#include <Wire.h>

#include "SparkFun_ExternalMemory.h"
ExternalMemory myMem;

#define CS_PIN  10

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("I2C EEPROM example");

  randomSeed(analogRead(A15));
  int randomLocation;

  Wire.begin();
  Wire.setClock(400000);

  //We need to assign the interface type in the constructor so that ...?

//  SPI.begin(); //Start SPI hardware before we start the EEPROM

//  if (myMem.begin(CS_PIN, SPI) == false) //Give the library the CS pin and SPI hardware to use
  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1);
  }
  Serial.println("Memory detected!");

  myMem.setMemorySize(512000/8);
//  myMem.setMemorySize(32000000 / 8);
//  myMem.setSPIFrequency(4000000);

  bool allTestsPassed = true;

  Serial.print("Mem size in bytes: ");
  Serial.println(myMem.length());

  myMem.getMfgID();

  //myMem.eraseSector(0);
  //myMem.eraseBlock(0);

  //Erase test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.println("Erase time is 10 to 50s: ");
  long startTime = millis();
  myMem.erase();
  long endTime = millis();
  Serial.printf("Time to erase all EEPROM: %dms\n", endTime - startTime);
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
  Serial.printf("Time to record byte: %d us\n", endTime - startTime);

  startTime = micros();
  myMem.write(randomLocation, myValue1); //(location, data)
  endTime = micros();
  Serial.printf("Time to write identical byte to same location (should be ~0ms): %d us\n", endTime - startTime);

  startTime = micros();
  byte response1 = myMem.read(randomLocation);
  endTime = micros();
  Serial.printf("Time to read byte: %d us\n", endTime - startTime);

  byte response2 = myMem.read(randomLocation + 1);
  Serial.printf("Location %d should be %d: %d\n\r", randomLocation, myValue1, response1);
  Serial.printf("Location %d should be %d: %d\n\r", randomLocation + 1, myValue2, response2);
  if (myValue1 != response1) allTestsPassed = false;
  if (myValue2 != response2) allTestsPassed = false;
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
  Serial.printf("Time to record int16: %d us\n", endTime - startTime);

  myMem.put(randomLocation + 2, myValue4);

  uint16_t response3;
  int16_t response4;

  startTime = micros();
  myMem.get(randomLocation, response3);
  endTime = micros();
  Serial.printf("Time to read int16: %d us\n", endTime - startTime);

  myMem.get(randomLocation + 2, response4);
  Serial.printf("Location %d should be %d: %d\n\r", randomLocation, myValue3, response3);
  Serial.printf("Location %d should be %d: %d\n\r", randomLocation + 2, myValue4, response4);
  if (myValue3 != response3) allTestsPassed = false;
  if (myValue4 != response4) allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("32 bit tests");

  //int and unsigned int (32) tests
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.printf("Size of int: %d\n", sizeof(int));
  int myValue5 = -245000;
  unsigned int myValue6 = 400123;
  randomLocation = random(0, myMem.length());

  startTime = micros();
  myMem.put(randomLocation, myValue5);
  endTime = micros();
  Serial.printf("Time to record int32: %d us\n", endTime - startTime);
  myMem.put(randomLocation + 4, myValue6);

  int response5;
  unsigned int response6;

  startTime = micros();
  myMem.get(randomLocation, response5);
  endTime = micros();
  Serial.printf("Time to read int32: %d us\n", endTime - startTime);

  myMem.get(randomLocation + 4, response6);
  Serial.printf("Location %d should be %d: %d\n\r", randomLocation, myValue5, response5);
  Serial.printf("Location %d should be %d: %d\n\r", randomLocation + 4, myValue6, response6);
  if (myValue5 != response5) allTestsPassed = false;
  if (myValue6 != response6) allTestsPassed = false;
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
  Serial.printf("Time to read int32: %d us\n", endTime - startTime);

  myMem.get(randomLocation + 4, response8);
  Serial.printf("Location %d should be %d: %d\n\r", randomLocation, myValue7, response7);
  Serial.printf("Location %d should be %d: %d\n\r", randomLocation + 4, myValue8, response8);
  if (myValue7 != response7) allTestsPassed = false;
  if (myValue8 != response8) allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //float (32) sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.printf("Size of float: %d\n", sizeof(float));
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
  Serial.printf("Time to read float: %d us\n", endTime - startTime);

  myMem.get(randomLocation + 4, response10);
  Serial.printf("Location %d should be %f: %f\n\r", randomLocation, myValue9, response9);
  Serial.printf("Location %d should be %f: %f\n\r", randomLocation + 4, myValue10, response10);
  if (myValue9 != response9) allTestsPassed = false;
  if (myValue10 != response10) allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("64 bit tests");

  //double (64) sequential test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  Serial.printf("Size of double: %d\n", sizeof(double));
  double myValue11 = -290.3485723409857;
  double myValue12 = 384.957;//34987;
  double myValue13 = 917.14159;
  double myValue14 = 254.8877;
  randomLocation = random(0, myMem.length());

  startTime = micros();
  myMem.put(randomLocation, myValue11);
  endTime = micros();
  Serial.printf("Time to record 64-bits: %d us\n", endTime - startTime);

  myMem.put(randomLocation + 8, myValue12);
  myMem.put(myMem.length() - sizeof(myValue13), myValue13); //Test end of EEPROM space

  double response11;
  double response12;
  double response13;

  startTime = micros();
  myMem.get(randomLocation, response11);
  endTime = micros();
  Serial.printf("Time to read 64-bits: %d us\n", endTime - startTime);

  myMem.get(randomLocation + 8, response12);
  myMem.get(myMem.length() - sizeof(myValue13), response13);
  Serial.printf("Location %d should be %lf: %lf\n", randomLocation, myValue11, response11);
  Serial.printf("Location %d should be %lf: %lf\n", randomLocation + 8, myValue12, response12);
  Serial.printf("Edge of EEPROM %d should be %lf: %lf\n", myMem.length() - sizeof(myValue13), myValue13, response13);

  double response14;
  myMem.put(myMem.length() - sizeof(myValue14), myValue14); //Test the re-write of a spot
  myMem.get(myMem.length() - sizeof(myValue14), response14);
  Serial.printf("Rewrite of %d should be %lf: %lf\n", myMem.length() - sizeof(myValue14), myValue14, response14);
  if (myValue11 != response11) allTestsPassed = false;
  if (myValue12 != response12) allTestsPassed = false;
  if (myValue13 != response13) allTestsPassed = false;
  if (myValue14 != response14) allTestsPassed = false;
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.println("String test");

  //Buffer write test
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  char myChars[242] = "Lorem ipsum dolor sit amet, has in verterem accusamus. Nulla viderer inciderint eum at. Quo elit nullam malorum te, agam fuisset detracto an sea, eam ut liber aperiri. Id qui velit facilisi. Mel probatus definitionem id, eu amet vidisse eum.";
  randomLocation = random(0, myMem.length());

  Serial.printf("Calculated time to record array: ~%dms\n", sizeof(myChars) / myMem.getPageSize() * myMem.getPageWriteTime());

  startTime = micros();
  myMem.put(randomLocation, myChars);
  endTime = micros();
  Serial.printf("Time to record array: %d us\n", endTime - startTime);

  char readMy[sizeof(myChars)];

  startTime = micros();
  myMem.get(randomLocation, readMy);
  endTime = micros();
  Serial.printf("Time to read array: %d us\n", endTime - startTime);

  Serial.printf("Location %d string should read: \n%s\n", randomLocation, myChars);
  Serial.println(readMy);
  if (strcmp(myChars, readMy) != 0) 
  {
    Serial.println("\nString compare failed");
    allTestsPassed = false;
  }
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  Serial.println("");
  Serial.print("Flash Contents:");
  for (uint16_t x = 0; x < 32 * 4; x ++)
  {
    if (x % 16 == 0)
      Serial.println();
    Serial.printf("0x%02X ", myMem.read(x));
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
