/*
  This is a library to read/write to external I2C EEPROMs.
  It uses the same template system found in the Arduino 
  EEPROM library so you can use the same get() and put() functions.

  https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library
  Best used with the Qwiic EEPROM: https://www.sparkfun.com/products/14764

  Various external EEPROMs have various interface specs
  (overall size, page size, write times, etc). This library works with
  all types and allows the various settings to be set at runtime.

  All read and write restrictions associated with pages are taken care of.
  You can access the external memory as if it was contiguous.

  Development environment specifics:
  Arduino IDE 1.8.x

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

*/

#ifndef _SPARKFUN_EXTERNAL_EEPROM_H
#define _SPARKFUN_EXTERNAL_EEPROM_H

#include "Arduino.h"
#include "Wire.h"

struct struct_memorySettings
{
  TwoWire *i2cPort;
  uint8_t deviceAddress;
  unsigned long memorySize_bytes;
  uint16_t pageSize_bytes;
  uint8_t pageWriteTime_ms;
  bool pollForWriteComplete;
};

class ExternalEEPROM
{
public:
  uint8_t read(uint32_t eepromLocation);
  void read(uint32_t eepromLocation, uint8_t *buff, uint16_t bufferSize);
  void write(uint32_t eepromLocation, uint8_t dataToWrite);
  void write(uint32_t eepromLocation, const uint8_t *dataToWrite, uint16_t blockSize);

  bool begin(uint8_t deviceAddress = 0b1010000, TwoWire &wirePort = Wire); //By default use the Wire port
  bool isConnected();
  bool isBusy();
  void erase(uint8_t toWrite = 0x00); //Erase the entire memory. Optional: write a given byte to each spot.

  //void settings(struct_memorySettings newSettings); //Set all the settings using the settings struct
  void setMemorySize(uint32_t memSize); //Set the size of memory in bytes
  uint32_t getMemorySize();             //Return size of EEPROM
  uint32_t length();                    //Return size of EEPROM
  void setPageSize(uint16_t pageSize);  //Set the size of the page we can write at a time
  uint16_t getPageSize();
  void setPageWriteTime(uint8_t writeTimeMS); //Set the number of ms required per page write
  uint8_t getPageWriteTime();

  //Functionality to 'get' and 'put' objects to and from EEPROM.
  template <typename T>
  T &get(int idx, T &t)
  {
    uint8_t *ptr = (uint8_t *)&t;
    read(idx, ptr, sizeof(T)); //Address, data, sizeOfData
    return t;
  }

  template <typename T>
  const T &put(int idx, const T &t) //Address, data
  {
    const uint8_t *ptr = (const uint8_t *)&t;
    write(idx, ptr, sizeof(T)); //Address, data, sizeOfData
    return t;
  }

private:
  //Variables
  struct_memorySettings settings = {
      .deviceAddress = 0b1010000, //0b1010 + (A2 A1 A0) or 0b1010 + (B0 A1 A0) for larger (>512kbit) EEPROMs
      .memorySize_bytes = 256000 / 8,
      .pageSize_bytes = 64,
      .pageWriteTime_ms = 5,
      .pollForWriteComplete = true,
  };
};

#endif //_SPARKFUN_EXTERNAL_EEPROM_H
