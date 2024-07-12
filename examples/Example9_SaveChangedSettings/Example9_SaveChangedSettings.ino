/*
  Read and write data to an external I2C EEPROM
  By: Nathan Seidle, Merlin Z
  SparkFun Electronics
  Date: July 11, 2024
  License: This code is public domain but you buy me a coffee if you use this
  and we meet someday (Beerware license).
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/18355

  This example demonstrates how to record various user settings easily to
  EEPROM, only writing settings that changed in order to preserve the life of
  the memory. Use the ExternalEEPROM::putChanged() method instead of
  External_EEPROM::put() to do this.

  The I2C EEPROM should have all its ADR pins set to GND (0). This is default
  on the Qwiic board.

  Hardware Connections:
  Plug the SparkFun Qwiic EEPROM to an Uno, Artemis, or other Qwiic equipped
  board.
  Load this sketch.
  Open serial monitor window with baud rate at 115200 and line ending set to
  Line Feed.
*/

// Include the necessary libraries
#include <Wire.h>

// Click here to get the library:
// http://librarymanager/All#SparkFun_External_EEPROM
// or download the zip here:
// https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/archive/master.zip
#include "SparkFun_External_EEPROM.h"

// Declare the EEPROM object
ExternalEEPROM myMem;

// This is the position in EEPROM to store the settings struct
#define LOCATION_SETTINGS 0

// This is the data that we use to validate the EEPROM data.
// The __TIME__ macro is the time the sketch was compiled, in HH:MM:SS format.
// This means that if the sketch is recompiled, the EEPROM data will be
// considered invalid,
#define VALID_DATA __TIME__

// This is the struct that contains all the settings.
// Add as many vars as you want.
typedef struct {
  // This first buffer is used to determine if the EEPROM has
  // been written to with valid data. If the bytes match the VALID_DATA
  // constant, then we can assume the data is valid.
  char memoryIsValid[sizeof(VALID_DATA)];
  // A baud rate setting (doesn't actually do anything in this example)
  unsigned long baudRate;
  // A setting to enable or disable the built-in LED
  bool enableLED;
} Settings;

// This is the settings object.
Settings settings;

void setup() {
  // Set up serial communication
  Serial.begin(115200);
  // delay(250); // Often needed for ESP based platforms

  while (!Serial.available()) {
    Serial.println(F("Press any key to begin."));
    delay(1000);
  }
  Serial.println(F("EEPROM load/save example"));

  // Initialize I2C
  Wire.begin();

  // Configure the EEPROM
  // Default to the Qwiic 24xx512 EEPROM:
  // https://www.sparkfun.com/products/18355
  // Valid types: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1025, 2048
  // For example, to use a 24C02 EEPROM, set the memory type to 2
  // Make sure to set this correctly or this example won't work!
  myMem.setMemoryType(2);

  // Initialize the EEPROM
  if (myMem.begin() == false) {
    Serial.println(F("No memory detected. Freezing."));
    while (true);
  }
  Serial.println(F("Memory detected!"));

  // Print out the size of the user settings struct
  Serial.print("Size of user settings in bytes: ");
  Serial.println(sizeof(settings));

  Serial.println("Settings are not initialized or loaded from EEPROM.");

  // Present user menu
  mainMenu();
}

void loop() {
  if (Serial.available()) mainMenu();  // Present user menu
}

void mainMenu() {
  int number;
  while (1) {
    Serial.println();
    Serial.println("Main menu:");
    Serial.println();
    Serial.println("P: Print Settings");
    Serial.println("L: Load Settings");
    Serial.println("S: Save Settings");
    Serial.println("R: Reset Settings to Default");
    Serial.println("B: Set Baud Rate");
    Serial.println("T: Toggle LED Setting");
    Serial.println();
    Serial.print("Enter choice: ");

    byte selection = readByte();

    switch (selection) {
      case 'p':
      case 'P':
        printSettings();
        break;
      case 'l':
      case 'L':
        loadSettings();
        break;
      case 's':
      case 'S':
        saveSettings();
        break;
      case 'r':
      case 'R':
        resetSettings();
        break;
      case 'b':
      case 'B':
        Serial.print("Enter baud rate (1200 to 115200): ");
        number = readNumber();
        if (number < 1200 || number > 115200) {
          Serial.println("Error: baud rate out of range");
        } else {
          settings.baudRate = number;
        }
        break;
      case 't':
      case 'T':
        settings.enableLED = !settings.enableLED;
        syncLED();
        Serial.print("LED is now ");
        Serial.println(settings.enableLED ? "on" : "off");
        break;
      default:
        Serial.print("Unknown choice: ");
        Serial.write(selection);
        Serial.println();
    }
  }
}

// Blocking wait for user input
void waitForInput() {
  delay(10);  // Wait for any incoming chars to hit buffer
  while (Serial.available() > 0) Serial.read();  // Clear buffer
  while (Serial.available() == 0);
}

// Get single byte from user.
// Waits for and returns the character that the user provides
byte readByte() {
  waitForInput();
  return (Serial.read());
}

// Get a string/value from user, remove all non-numeric values
int readNumber() {
  waitForInput();

  char numberString[20 + 1];  // Room for 20 digits plus terminating \0

  int index = 0;
  while (index < 20) {
    // Wait for user input
    while (Serial.available() == 0);

    // Read the incoming byte
    byte incoming = Serial.read();

    // If the user hits enter, we're done
    if (incoming == '\n' || incoming == '\r') {
      Serial.println();
      break;
    }

    // Check for a digit
    if (isDigit(incoming) == true) {
      // Echo the digit
      Serial.write(incoming);
      // Add the digit to the string
      numberString[index] = (char)incoming;
      index++;
    }
  }

  // Null terminate the string
  numberString[index] = '\0';

  // Convert the string to an integer and return it
  String tempValue = numberString;
  return (tempValue.toInt());
}

// This function is called to sync the LED state with the settings
// (On my board at least), the LED is active low, so we invert the setting
void syncLED() { digitalWrite(PIN_LED, !settings.enableLED); }

// Reset the settings struct to default values (without writing to EEPROM)
void resetSettings() {
  // Set the first 8 bytes of the settings struct to the VALID_DATA constant
  memcpy(settings.memoryIsValid, VALID_DATA, sizeof(settings.memoryIsValid));
  // Set the baud rate to 9600
  settings.baudRate = 9600;
  // Set the LED to off
  settings.enableLED = false;
  syncLED();

  Serial.println("Settings reset to default.");
}

// Load the current settings from EEPROM into the settings struct
void loadSettings() {
  // Call the get() method with the location and the struct to load into
  myMem.get(LOCATION_SETTINGS, settings);

  // Sync the LED with the settings
  syncLED();

  Serial.println("Settings loaded from EEPROM.");

  if (memcmp(settings.memoryIsValid, VALID_DATA,
             sizeof(settings.memoryIsValid)) != 0) {
    Serial.println(
        "Memory is not valid. You should reset the settings and save them.");
    // In a real application, you would do this here:
    // resetSettings();
    // saveSettings();
  } else {
    Serial.println("Memory is valid!");
  }
}

// Record the current settings into EEPROM
void saveSettings() {
  // The putChanged() method writes the data to the EEPROM
  // (the same as the put() method except it only writes data that changed).
  myMem.put(LOCATION_SETTINGS, settings);

  Serial.println("Settings saved to EEPROM.");
}

// Print the current settings to the serial monitor
void printSettings() {
  Serial.println("Current settings:");
  Serial.println();

  Serial.print("Memory valid: ");
  for (int i = 0; i < sizeof(settings.memoryIsValid); i++) {
    Serial.print(settings.memoryIsValid[i], 16);
    Serial.print(' ');
  }
  Serial.println();
  Serial.print("  VALID_DATA: ");
  for (int i = 0; i < sizeof(settings.memoryIsValid); i++) {
    Serial.print(VALID_DATA[i], 16);
    Serial.print(' ');
  }
  Serial.println();

  Serial.print("Baud rate: ");
  Serial.println(settings.baudRate);

  Serial.print("LED: ");
  Serial.println(settings.enableLED ? "on" : "off");
}
