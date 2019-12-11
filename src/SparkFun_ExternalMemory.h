/*
  This is a library to read/write to external memories like an
  I2C or SPI external EEPROM or flash memory. It uses the same
  template system found in the Arduino EEPROM library written
  so you can use the same get() and put() functions.

  https://github.com/sparkfun/SparkFun_ExternalMemory_Arduino_Library

  Various external EEPROMs have various interface specs (I2C vs SPI,
  overall size, page size, write times, etc). This library works with
  all types and allows the various settings to be set at runtime.

  All read and write restrictions associated with pages are taken care of.
  You can access the external memory as if it was contiguous.

  Development environment specifics:
  Arduino IDE 1.8.x

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _SPARKFUN_EXTERNALMEMORY_H
#define _SPARKFUN_EXTERNALMEMORY_H

#include "Arduino.h"
#include "Wire.h"
#include "SPI.h"

typedef enum
{
  INTERFACE_I2C,
  INTERFACE_SPI,
} interface_t;

struct struct_memorySettings {
  interface_t interfaceType;
  TwoWire *i2cPort;
  SPIClass *spiPort;
  SPISettings spiSettings;
  uint8_t deviceAddress;
  unsigned long memorySize_bytes;
  uint16_t pageSize_bytes;
  uint8_t pageWriteTime_ms;
  bool pollForWriteComplete;
  uint8_t csPin;
};

//SPI setting defaults
#define EXTERNALMEMORY_SPI_DEFAULT_FREQ 4000000
#define EXTERNALMEMORY_SPI_DEFAULT_ORDER MSBFIRST
#define EXTERNALMEMORY_SPI_DEFAULT_MODE SPI_MODE0

//SPI EEPROM Commands
enum spiCommands {
  EXTERNALMEM_COMMAND_PAGE_PROGRAM = 0x02,
  EXTERNALMEM_COMMAND_READ_DATA_BYTES = 0x03,
  EXTERNALMEM_COMMAND_WRITE_DISABLE = 0x04,
  EXTERNALMEM_COMMAND_READ_STATUS = 0x05,
  EXTERNALMEM_COMMAND_WRITE_ENABLE = 0x06,
  EXTERNALMEM_COMMAND_SECTOR_ERASE = 0x20,
  EXTERNALMEM_COMMAND_BLOCK_ERASE = 0x52,
  EXTERNALMEM_COMMAND_READ_ID = 0x9F,
  EXTERNALMEM_COMMAND_CHIP_ERASE = 0xC7,
};

class ExternalMemory
{
  public:
    uint8_t read(uint32_t eepromLocation);
    void read(uint32_t eepromLocation, uint8_t *buff, uint16_t bufferSize);
    void write(uint32_t eepromLocation, uint8_t dataToWrite);
    void write(uint32_t eepromLocation, const uint8_t *dataToWrite, uint16_t blockSize);

    bool begin(TwoWire &wirePort = Wire); //By default use the Wire port
    bool begin(uint8_t csPin, SPIClass &spiPort);
    bool isConnected();
    bool isBusy();
    void erase(); //Erase the entire memory
    void eraseSector(uint32_t sectorNumber); //Erase a given 4k byte sector
    void eraseBlock(uint32_t blockNumber); //Erase a given 32k byte block

    //void settings(struct_memorySettings newSettings); //Set all the settings using the settings struct
    void setDeviceAddress(uint8_t deviceAddress); //Set the I2C address of the device we want to talk to
    void setMemorySize(uint32_t memSize); //Set the size of memory in bytes
    uint32_t getMemorySize(); //Return size of EEPROM
    uint32_t length(); //Return size of EEPROM
    void setPageSize(uint16_t pageSize); //Set the size of the page we can write at a time
    uint16_t getPageSize();
    void setPageWriteTime(uint8_t writeTimeMS); //Set the number of ms required per page write
    uint8_t getPageWriteTime();

    uint8_t getMfgID();

    void setSPIFrequency(uint32_t spiFreq); //Max SPI speed for this EEPROM
    //uint32_t getSPIFrequecy(); Not easily exposed since spiFreq gets stored directly into SPISettings

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
      .interfaceType = INTERFACE_I2C,
      .spiSettings = SPISettings(EXTERNALMEMORY_SPI_DEFAULT_FREQ, EXTERNALMEMORY_SPI_DEFAULT_ORDER, EXTERNALMEMORY_SPI_DEFAULT_MODE),
      .deviceAddress = 0b1010000, //0b1010 + (A2 A1 A0) or 0b1010 + (B0 A1 A0) for larger (>512kbit) EEPROMs
      .memorySize_bytes = 256000 / 8,
      .pageSize_bytes = 64,
      .pageWriteTime_ms = 5,
      .pollForWriteComplete = true,
      .csPin = 255,
    };
};

#endif //_SPARKFUN_EXTERNALMEMORY_H
