/*
  This is a library written for the SparkFun Artemis

  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support open source hardware. Buy a board!
  https://www.sparkfun.com/products/15332
  https://www.sparkfun.com/products/15376
  https://www.sparkfun.com/products/15411
  https://www.sparkfun.com/products/15412

  Written by Nathan Seidle @ SparkFun Electronics, June 16th, 2019

  Pseudo-EEPROM on the Cortex-M4eF

  https://github.com/sparkfun/SparkFun_Apollo3

  There is no EEPROM built into the Apollo3 Cortex-M4F but we have 1M of flash
  so we carve out the last 8k of flash for EEPROM space. Pages are erased in
  8k chunks so 8k of EEPROM works.

  Flash is 0x00 to 0xFF000. EEPROM writes will start at 0xFF000 - 8192 = 0xF2000.

  Page erase takes 15ms
  Writing a byte takes 19ms
  Writing a float across two words takes 19ms
  Update (no write) takes ~1ms

  Development environment specifics:
  Arduino IDE 1.8.x

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Arduino.h"
#include "SparkFun_ExternalMemory.h"
#include "Wire.h"



bool ExternalMemory::begin(TwoWire &wirePort)
{
  settings.interfaceType = INTERFACE_I2C;
  settings.i2cPort = &wirePort; //Grab which port the user wants us to use

  if (isConnected() == false)
  {
    return false;
  }

  return true;
}

bool ExternalMemory::begin(uint8_t csPin, SPIClass &spiPort)
{
  settings.interfaceType = INTERFACE_SPI;

  //Grab user settings
  settings.spiPort = &spiPort;
  settings.csPin = csPin;

  pinMode(settings.csPin, OUTPUT);
  digitalWrite(settings.csPin, HIGH); //Deselect the device

  // 'Kickstart' the SPI hardware.
  settings.spiPort->beginTransaction(settings.spiSettings);
  settings.spiPort->transfer(0x00);
  settings.spiPort->endTransaction();

  //  if (isConnected() == false)
  //  {
  //    return false;
  //  }

  return true;
}

//Erase entire EEPROM
void ExternalMemory::erase()
{
  if (settings.interfaceType == INTERFACE_I2C)
  {
    uint8_t tempBuffer[settings.pageSize_bytes];
    for (uint32_t x = 0 ; x < settings.pageSize_bytes ; x++)
      tempBuffer[x] = 0xFF;

    for (uint32_t addr = 0 ; addr < length() ; addr += settings.pageSize_bytes)
      write(addr, tempBuffer, settings.pageSize_bytes);
  }
  else
  {
    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_WRITE_ENABLE); //Sets the WEL bit to 1
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);

    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_CHIP_ERASE); //Erase entire chip
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);

    while (isBusy() == true)
      delay(50);
  }
}

void ExternalMemory::eraseSector(uint32_t sectorNumber)
{
  if (settings.interfaceType == INTERFACE_SPI)
  {
    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_WRITE_ENABLE); //Sets the WEL bit to 1
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);

    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_SECTOR_ERASE); //Erase sector
    SPI.transfer(sectorNumber >> 16);
    SPI.transfer(sectorNumber >> 8);
    SPI.transfer((uint8_t)sectorNumber);
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);

    while (isBusy() == true)
      delay(50);
  }
}

void ExternalMemory::eraseBlock(uint32_t blockNumber)
{
  if (settings.interfaceType == INTERFACE_SPI)
  {
    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_WRITE_ENABLE); //Sets the WEL bit to 1
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);

    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_SECTOR_ERASE); //Erase sector
    SPI.transfer(blockNumber >> 16);
    SPI.transfer(blockNumber >> 8);
    SPI.transfer((uint8_t)blockNumber);
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);

    while (isBusy() == true)
      delay(50);
  }
}

uint32_t ExternalMemory::length()
{
  return settings.memorySize_bytes;
}

//Returns true if device is detected
bool ExternalMemory::isConnected()
{
  if (settings.interfaceType == INTERFACE_I2C)
  {
    settings.i2cPort->beginTransmission((uint8_t)settings.deviceAddress);
    if (settings.i2cPort->endTransmission() == 0)
      return (true);
  }
  return (false);
}

//Returns true if device is not answering (currently writing)
bool ExternalMemory::isBusy()
{
  if (settings.interfaceType == INTERFACE_I2C)
  {
    if (isConnected()) return (false);
    return (true);
  }
  else
  {
    digitalWrite(settings.csPin, LOW);
    SPI.transfer(EXTERNALMEM_COMMAND_READ_STATUS); //Read status byte 1
    byte status = SPI.transfer(0xFF);
    digitalWrite(settings.csPin, HIGH);

    //Serial.print("eepromReadStatus1: 0x");
    //Serial.println(status, HEX);

    if (status & 0x01) return (true);
    return (false);
  }
}

//void ExternalMemory::settings(struct_memorySettings newSettings)
//{
//  settings.deviceAddress = newSettings.deviceAddress;
//}
void ExternalMemory::setDeviceAddress(uint8_t primaryAddr)
{
  settings.deviceAddress = primaryAddr;
}
void ExternalMemory::setMemorySize(uint32_t memSize)
{
  settings.memorySize_bytes = memSize;
}
uint32_t ExternalMemory::getMemorySize()
{
  return settings.memorySize_bytes;
}
void ExternalMemory::setPageSize(uint16_t pageSize)
{
  settings.pageSize_bytes = pageSize;
}
uint16_t ExternalMemory::getPageSize()
{
  return settings.pageSize_bytes;
}
void ExternalMemory::setPageWriteTime(uint8_t writeTimeMS)
{
  settings.pageWriteTime_ms = writeTimeMS;
}
uint8_t ExternalMemory::getPageWriteTime()
{
  return settings.pageWriteTime_ms;
}
void ExternalMemory::setSPIFrequency(uint32_t spiFreq)
{
  settings.spiSettings = SPISettings(spiFreq, EXTERNALMEMORY_SPI_DEFAULT_ORDER, EXTERNALMEMORY_SPI_DEFAULT_MODE);
}

uint8_t ExternalMemory::getMfgID()
{
  if (settings.interfaceType == INTERFACE_SPI)
  {
    //Begin reading
    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_READ_ID); //Read manufacturer and device ID
    byte manuID = SPI.transfer(0xFF);
    byte deviceID1 = SPI.transfer(0xFF);
    byte deviceID2 = SPI.transfer(0xFF);
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);

    Serial.print("manuID: 0x");
    Serial.println(manuID, HEX);
    return (manuID);
  }
  return (0); //No ID
}

//Read a byte from a given location
uint8_t ExternalMemory::read(uint32_t eepromLocation)
{
  //    uint8_t i2cAddress = settings.deviceAddress;
  //
  //    //Check if we are dealing with large (>512kbit) EEPROMs
  //    if (settings.memorySize_bytes > 0xFFFF)
  //    {
  //      //Figure out if we are accessing the lower half or the upper half
  //      if (eepromLocation > 0xFFFF)
  //        i2cAddress |= 0b100; //Set the block bit to 1
  //    }
  //
  //    settings.i2cPort->beginTransmission(i2cAddress);
  //    settings.i2cPort->write((uint8_t)(eepromLocation >> 8)); // MSB
  //    settings.i2cPort->write((uint8_t)(eepromLocation & 0xFF)); // LSB
  //    settings.i2cPort->endTransmission();
  //
  //    settings.i2cPort->requestFrom(i2cAddress, 1);
  //
  //    byte rdata = 0xFF;
  //    if (settings.i2cPort->available()) rdata = settings.i2cPort->read();
  //    return rdata;

  uint8_t tempByte;
  read(eepromLocation, &tempByte, 1);
  return tempByte;
}

//Bulk read from EEPROM
//Handles breaking up read amt into 32 byte chunks
//Handles a read that straddles the 512kbit barrier
void ExternalMemory::read(uint32_t eepromLocation, uint8_t *buff, uint16_t bufferSize)
{
  if (settings.interfaceType == INTERFACE_I2C)
  {
    uint16_t received = 0;
    while (received < bufferSize)
    {
      //Limit the amount to write to a page size
      int amtToRead = bufferSize - received;
      if (amtToRead > 32) //Arduino I2C buffer size limit
        amtToRead = 32;

      //Check if we are dealing with large (>512kbit) EEPROMs
      uint8_t i2cAddress = settings.deviceAddress;
      if (settings.memorySize_bytes > 0xFFFF)
      {
        //Figure out if we are going to cross the barrier with this read
        if (0xFFFF - (eepromLocation + received) < amtToRead) //0xFFFF - 0xFFFA < 32
          amtToRead = 0xFFFF - (eepromLocation + received); //Limit the read amt to go right up to edge of barrier

        //Figure out if we are accessing the lower half or the upper half
        if (eepromLocation + received > 0xFFFF)
          i2cAddress |= 0b100; //Set the block bit to 1
      }

      settings.i2cPort->beginTransmission(i2cAddress);
      settings.i2cPort->write((uint8_t)((eepromLocation + received) >> 8)); // MSB
      settings.i2cPort->write((uint8_t)((eepromLocation + received) & 0xFF)); // LSB
      settings.i2cPort->endTransmission();

      settings.i2cPort->requestFrom(i2cAddress, amtToRead);

      for (uint16_t x = 0 ; x < amtToRead ; x++)
        buff[received + x] = settings.i2cPort->read();

      settings.i2cPort->endTransmission(); //Send stop condition

      received += amtToRead;
    }
  }
  else
  { //SPI
    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_READ_DATA_BYTES); //Read command, no dummy bytes
    SPI.transfer(eepromLocation >> 16); //Address byte MSB
    SPI.transfer(eepromLocation >> 8); //Address byte MMSB
    SPI.transfer(eepromLocation & 0xFF); //Address byte LSB
    for (int x = 0 ; x < bufferSize ; x++)
      buff[x] = SPI.transfer(0xFF);
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);
  }
}

//Write a byte to a given location
void ExternalMemory::write(uint32_t eepromLocation, uint8_t dataToWrite)
{
  if (read(eepromLocation) != dataToWrite) //Update only if data is new
    write(eepromLocation, &dataToWrite, 1);
}

//Write large bulk amounts
void ExternalMemory::write(uint32_t eepromLocation, const uint8_t *dataToWrite, uint16_t bufferSize)
{
  if (settings.interfaceType == INTERFACE_I2C)
  {
    //Error check
    if (eepromLocation + bufferSize >= settings.memorySize_bytes)
      bufferSize = settings.memorySize_bytes - eepromLocation;

    //Break the buffer into page sized chunks
    uint16_t recorded = 0;
    while (recorded < bufferSize)
    {
      //Limit the amount to write to a page size
      int amtToWrite = bufferSize - recorded;
      if (amtToWrite > settings.pageSize_bytes)
        amtToWrite = settings.pageSize_bytes;

      if (amtToWrite > 1)
      {
        //Writes cannot cross a page line
        uint16_t pageNumber1 = (eepromLocation + recorded) / settings.pageSize_bytes;
        uint16_t pageNumber2 = (eepromLocation + recorded + amtToWrite - 1) / settings.pageSize_bytes;
        if (pageNumber2 > pageNumber1)
          amtToWrite = (pageNumber2 * settings.pageSize_bytes) - (eepromLocation + recorded); //Limit the read amt to go right up to edge of page barrier
      }

      //Check if we are dealing with large (>512kbit) EEPROMs
      uint8_t i2cAddress = settings.deviceAddress;
      if (settings.memorySize_bytes > 0xFFFF)
      {
        //Figure out if we are accessing the lower half or the upper half
        if (eepromLocation + recorded > 0xFFFF)
          i2cAddress |= 0b100; //Set the block bit to 1
      }
      settings.i2cPort->beginTransmission(i2cAddress);

      settings.i2cPort->write((uint8_t)((eepromLocation + recorded) >> 8)); // MSB
      settings.i2cPort->write((uint8_t)((eepromLocation + recorded) & 0xFF)); // LSB

      for (uint8_t x = 0 ; x < amtToWrite ; x++)
        settings.i2cPort->write(dataToWrite[recorded + x]);

      settings.i2cPort->endTransmission(); //Send stop condition

      recorded += amtToWrite;

      if (settings.pollForWriteComplete == true)
      {
        while (isBusy() == true) //Poll device
          delay(1);
      }
      else
        delay(settings.pageWriteTime_ms); //Delay the amount of time to record a page
    }
  }
  else
  { //SPI
    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_WRITE_ENABLE); //Sets the WEL bit to 1
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);

    digitalWrite(settings.csPin, LOW);
    SPI.beginTransaction(settings.spiSettings);
    SPI.transfer(EXTERNALMEM_COMMAND_PAGE_PROGRAM); //Byte/Page program
    SPI.transfer(eepromLocation >> 16); //Address byte MSB
    SPI.transfer(eepromLocation >> 8); //Address byte MMSB
    SPI.transfer(eepromLocation & 0xFF); //Address byte LSB
    for (int x = 0 ; x < bufferSize ; x++)
      SPI.transfer(dataToWrite[x]);
    SPI.endTransaction();
    digitalWrite(settings.csPin, HIGH);
  }
}
