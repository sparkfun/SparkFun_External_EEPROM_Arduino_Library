/*
  Read and write settings and calibration data to an external I2C EEPROM
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 11th, 2019
  License: This code is public domain but you buy me a beer if you use this
  and we meet someday (Beerware license).
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14764

  This example demonstrates how to record various user settings easily to EEPROM.

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

#define LOCATION_SETTINGS 0 //Position in EEPROM to store the user setting structure

//This is the struct that contains all the user settings. Add as many vars as you want.
struct struct_userSettings {
  unsigned long baudRate;
  bool logDate;
  bool enableIMU;
  float calValue;
};

//These are the default settings for each variable. They will be written if the EEPROM is blank.
struct_userSettings settings = {
  .baudRate = 115200,
  .logDate = false,
  .enableIMU = true,
  .calValue = -5.17,
};

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println(F("Qwiic EEPROM example"));

  Wire.begin();

  if (myMem.begin() == false)
  {
    Serial.println(F("No memory detected. Freezing."));
    while (1);
  }
  Serial.println(F("Memory detected!"));

  Serial.print("Size of user settings (bytes): ");
  Serial.println(sizeof(settings));

  loadUserSettings();

  Serial.print("Baud rate: ");
  Serial.println(settings.baudRate);

  Serial.print("logDate: ");
  if (settings.logDate == true) Serial.println("True");
  else Serial.println("False");

  Serial.print("calValue: ");
  Serial.println(settings.calValue, 2);

  //Now we can change something
  settings.baudRate = 57600;

  //Now we can save it
  recordUserSettings();

  //And we never have to worry about byte alignment or EEPROM locations!

  Serial.println("Press any key to get menu");
}

void loop()
{
  if (Serial.available()) mainMenu(); //Present user menu
}

void mainMenu()
{
  while (1)
  {
    Serial.println();
    Serial.println("Menu: Main Menu");

    Serial.println("1) Set Baud Rate");
    Serial.println("x) Exit");

    byte incoming = getByteChoice(); //Timeout after 10 seconds

    if (incoming == '1')
    {
      Serial.print("Enter baud rate (1200 to 115200): ");
      int newBaud = getNumber();
      if (newBaud < 1200 || newBaud > 115200)
      {
        Serial.println("Error: baud rate out of range");
      }
      else
      {
        settings.baudRate = newBaud;
        recordUserSettings();
      }
    }
    else if (incoming == 'x')
      break;
    else
    {
      Serial.print("Unknown choice: ");
      Serial.write(incoming);
      Serial.println();
    }
  }
}

//Blocking wait for user input
void waitForInput()
{
  delay(10); //Wait for any incoming chars to hit buffer
  while (Serial.available() > 0) Serial.read(); //Clear buffer
  while (Serial.available() == 0);
}

//Get single byte from user
//Waits for and returns the character that the user provides
byte getByteChoice()
{
  waitForInput(); //Wait for user to send a value
  return (Serial.read());
}

//Get a string/value from user, remove all non-numeric values
int getNumber()
{
  waitForInput(); //Wait for user to send a value

  //Get input from user
  char cleansed[20];

  int spot = 0;
  while (spot < 20 - 1) //Leave room for terminating \0
  {
    while (Serial.available() == 0) ; //Wait for user input

    byte incoming = Serial.read();
    if (incoming == '\n' || incoming == '\r')
    {
      Serial.println();
      break;
    }

    if (isDigit(incoming) == true)
    {
      Serial.write(incoming); //Echo user's typing
      cleansed[spot++] = (char)incoming;
    }
  }

  cleansed[spot] = '\0';

  String tempValue = cleansed;
  return (tempValue.toInt());
}


//Load the current settings from EEPROM into the settings struct
void loadUserSettings()
{
  //Uncomment these lines to forcibly erase the EEPROM and see how the defaults are set
  //Serial.println("Erasing EEPROM");
  //myMem.erase();

  //Check to see if EEPROM is blank. If the first four spots are zeros then we can assume the EEPROM is blank.
  uint32_t testRead = 0;
  if (myMem.get(LOCATION_SETTINGS, testRead) == 0) //EEPROM address to read, thing to read into
  {
    //At power on, settings are set to defaults within the struct.
    //So go record the struct as it currently exists so that defaults are set.
    recordUserSettings();

    Serial.println("Default settings applied");
  }
  else
  {
    //Read current settings
    myMem.get(LOCATION_SETTINGS, settings);
  }
}

//Record the current settings into EEPROM
void recordUserSettings()
{
  myMem.put(LOCATION_SETTINGS, settings); //That'
}
