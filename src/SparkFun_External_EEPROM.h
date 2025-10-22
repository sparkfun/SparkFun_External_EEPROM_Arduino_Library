/*
  This is a library to read/write to external I2C EEPROMs.
  It uses the same template system found in the Arduino
  EEPROM library so you can use the same get() and put() functions.

  https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library
  Best used with the Qwiic EEPROM: https://www.sparkfun.com/products/18355

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

#if defined(ARDUINO_ARCH_APOLLO3)

#define I2C_BUFFER_LENGTH_RX                                                                                           \
    256 // Hardcoding until issue is resolved: https://github.com/sparkfun/Arduino_Apollo3/issues/351
#define I2C_BUFFER_LENGTH_TX 256

#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

#define I2C_BUFFER_LENGTH_RX BUFFER_LENGTH // I2C_BUFFER_LENGTH is defined in Wire.H
#define I2C_BUFFER_LENGTH_TX BUFFER_LENGTH

#elif defined(__SAMD21G18A__) || defined(__SAMD21E18A__)

#define I2C_BUFFER_LENGTH_RX SERIAL_BUFFER_SIZE // SAMD21 uses RingBuffer.h
#define I2C_BUFFER_LENGTH_TX SERIAL_BUFFER_SIZE

#elif defined(__SAMD51__) || defined(ADAFRUIT_METRO_M4_EXPRESS)

// SAMD51 doesn't have a definition, but it does have this as a template size
// in Wire.h: https://github.com/adafruit/ArduinoCore-samd/blob/master/libraries/Wire/Wire.h#L74
#define I2C_BUFFER_LENGTH_RX 256
#define I2C_BUFFER_LENGTH_TX 256

#elif (defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || defined(__MK64FX512__) ||          \
       defined(__MK66FX1M0__) || defined(__IMXRT1062__)) // 3.0/3.1-3.2/LC/3.5/3.6/4.0

#define I2C_BUFFER_LENGTH_RX BUFFER_LENGTH // Teensy
#define I2C_BUFFER_LENGTH_TX BUFFER_LENGTH

#elif defined(ESP32)

#define I2C_BUFFER_LENGTH_RX I2C_BUFFER_LENGTH
#define I2C_BUFFER_LENGTH_TX I2C_BUFFER_LENGTH

#elif defined(ESP8266)

#define I2C_BUFFER_LENGTH_RX BUFFER_LENGTH // BUFFER_LENGTH is defined in Wire.h for ESP8266
#define I2C_BUFFER_LENGTH_TX BUFFER_LENGTH

#elif defined(STM32)

#define I2C_BUFFER_LENGTH_RX BUFFER_LENGTH // BUFFER_LENGTH is defined in Wire.h for STM32
#define I2C_BUFFER_LENGTH_TX BUFFER_LENGTH

#elif defined(STM32_CORE_VERSION)

#define I2C_BUFFER_LENGTH_RX BUFFER_LENGTH // BUFFER_LENGTH is defined in Wire.h for STM32
#define I2C_BUFFER_LENGTH_TX BUFFER_LENGTH

#elif defined(NRF52_SERIES)

#define I2C_BUFFER_LENGTH_RX SERIAL_BUFFER_SIZE // Adafruit Bluefruit nRF52 Boards uses RingBuffer.h
#define I2C_BUFFER_LENGTH_TX SERIAL_BUFFER_SIZE

#elif defined(ARDUINO_ARCH_RP2040)

#ifdef WIRE_BUFFER_SIZE
#define I2C_BUFFER_LENGTH_RX WIRE_BUFFER_SIZE // 128 - defined in Wire.h (provided by pico-arduino-compat)
#define I2C_BUFFER_LENGTH_TX WIRE_BUFFER_SIZE
#elif defined(ARDUINO_RASPBERRY_PI_PICO)

#define I2C_BUFFER_LENGTH_RX                                                                                           \
    256 // Not properly defined but set at 256:
        // https://github.com/arduino/ArduinoCore-mbed/blob/master/libraries/Wire/Wire.h
#define I2C_BUFFER_LENGTH_TX 256
#else
#pragma GCC warning                                                                                                    \
    "This RP2040 platform doesn't have a wire buffer size defined. Defaulting to 32 bytes. Please contribute to this library!"

// Default to safe 32 bytes
#define I2C_BUFFER_LENGTH_RX 32
#define I2C_BUFFER_LENGTH_TX 32
#endif

#else

#pragma GCC warning                                                                                                    \
    "This platform doesn't have a wire buffer size defined. Defaulting to 32 bytes. Please contribute to this library!"

// Default to safe 32 bytes
#define I2C_BUFFER_LENGTH_RX 32
#define I2C_BUFFER_LENGTH_TX 32

#endif

struct struct_memorySettings
{
    TwoWire *i2cPort;
    uint8_t deviceAddress;
    uint32_t memorySize_bytes;
    uint16_t pageSize_bytes;
    uint8_t writeTime_ms;
    bool pollForWriteComplete;
    uint8_t addressSize_bytes;
    uint8_t wpPin;
};

class ExternalEEPROM
{
  public:
    uint8_t read(uint32_t eepromLocation);
    int read(uint32_t eepromLocation, uint8_t *buff, uint16_t bufferSize);
    int write(uint32_t eepromLocation, uint8_t dataToWrite);
    int write(uint32_t eepromLocation, const uint8_t *dataToWrite, uint16_t blockSize);

    bool begin(uint8_t deviceAddress = 0b1010000, TwoWire &wirePort = Wire, uint8_t WP = 255); // By default use the Wire port

    bool isConnected(uint8_t i2cAddress = 255);
    bool isBusy(uint8_t i2cAddress = 255);
    void erase(uint8_t toWrite = 0x00); // Erase the entire memory. Optional: write a given byte to each spot.

    // void settings(struct_memorySettings newSettings); //Set all the settings using the settings struct

    uint32_t detectMemorySizeBytes();          // Attempts to detect the size of the EEPROM
    void setMemorySizeBytes(uint32_t memSize); // Set the size of memory in bytes
    uint32_t getMemorySizeBytes();             // Return size of EEPROM
    void setMemorySize(uint32_t memSize);      // Depricated
    uint32_t getMemorySize();                  // Depricated
    uint32_t length();                         // Return size of EEPROM in bytes

    void setMemoryType(uint16_t typeNumber);      // Valid types: 00, 01, 02, 04, 08, 16, 32, 64, 128, 256, 512, 1025, 2048

    uint8_t detectAddressBytes(); // Determine the number of address bytes, 1 or 2
    void setAddressBytes(uint8_t addressBytes);
    uint8_t getAddressBytes();

    uint16_t detectPageSizeBytes();           // Returns the number of bytes successfully written in one page
    void setPageSizeBytes(uint16_t pageSize); // Set the size of the page we can write at a time
    uint16_t getPageSizeBytes();
    void setPageSize(uint16_t pageSize); // Depricated
    uint16_t getPageSize();              // Depricated

    uint8_t detectWriteTimeMs(uint8_t numberOfTests = 8);
    void setWriteTimeMs(uint8_t writeTimeMS); // Set the number of ms required per page write
    uint8_t getWriteTimeMs();
    void setPageWriteTime(uint8_t writeTimeMS); // Depricated
    uint8_t getPageWriteTime();                 // Depricated

    void enablePollForWriteComplete(); // Most EEPROMs all I2C polling of when a write has completed
    void disablePollForWriteComplete();
    constexpr uint16_t getI2CBufferSize(); // Return the size of the TX buffer

    // Functionality to 'get' and 'put' objects to and from EEPROM.
    template <typename T> T &get(uint32_t idx, T &t)
    {
        uint8_t *ptr = (uint8_t *)&t;
        read(idx, ptr, sizeof(T)); // Address, data, sizeOfData
        return t;
    }

    template <typename T> const T &put(uint32_t idx, const T &t) // Address, data
    {
        const uint8_t *ptr = (const uint8_t *)&t;
        write(idx, ptr, sizeof(T)); // Address, data, sizeOfData
        return t;
    }

    template <typename T> const T &putChanged(uint32_t idx, const T &t)  // Address, data
    {
      const uint8_t *newData = (const uint8_t *)&t;
      uint8_t oldData[sizeof(T)];
      read(idx, oldData, sizeof(T));  // Address, data, sizeOfData
      for (uint16_t i = 0; i < sizeof(T); i++) {
        if (oldData[i] != newData[i]) {
          write(idx + i, newData[i]);
        }
      }
      return t;
    }

    uint32_t putString(uint32_t eepromLocation, String &strToWrite);
    void getString(uint32_t eepromLocation, String &strToRead);

  private:
    // Default settings are for onsemi CAT24C51 512Kbit I2C EEPROM used on SparkFun Qwiic EEPROM Breakout
    struct_memorySettings settings = {
        .i2cPort = &Wire,
        .deviceAddress =
            0b1010000, // 0x50; format is 0b1010 + (A2 A1 A0) or 0b1010 + (B0 A1 A0) for larger (>512kbit) EEPROMs
        .memorySize_bytes = 4096, // Default to 4096, to support 24xx32 / 4096 byte EEPROMs and larger
        .pageSize_bytes = 32, // Default to 32 bytes, to support 24xx32 / 4096 byte EEPROMs and larger
        .writeTime_ms = 5, //All EEPROMs seem to have a max write time of 5ms
        .pollForWriteComplete = true,
        .addressSize_bytes = 2, // Default to two address bytes, to support 24xx32 / 4096 byte EEPROMs and larger
        .wpPin = 255, // By default, the write protection pin is not set
    };
};

#endif //_SPARKFUN_EXTERNAL_EEPROM_H
