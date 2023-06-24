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

#include "SparkFun_External_EEPROM.h"
#include "Arduino.h"
#include "Wire.h"

bool ExternalEEPROM::begin(uint8_t deviceAddress, TwoWire &wirePort)
{
    settings.i2cPort = &wirePort; // Grab which port the user wants us to use
    settings.deviceAddress = deviceAddress;

    if (isConnected() == false)
    {
        return false;
    }

    return true;
}

// Erase entire EEPROM
void ExternalEEPROM::erase(uint8_t toWrite)
{
    uint8_t tempBuffer[settings.pageSize_bytes];
    for (uint32_t x = 0; x < settings.pageSize_bytes; x++)
        tempBuffer[x] = toWrite;

    for (uint32_t addr = 0; addr < length(); addr += settings.pageSize_bytes)
        write(addr, tempBuffer, settings.pageSize_bytes);
}

uint32_t ExternalEEPROM::length()
{
    return settings.memorySize_bytes;
}

// Returns true if device is detected
bool ExternalEEPROM::isConnected(uint8_t i2cAddress)
{
    if (i2cAddress == 255)
        i2cAddress = settings.deviceAddress; // We can't set the default to settings.deviceAddress so we use 255 instead

    settings.i2cPort->beginTransmission((uint8_t)i2cAddress);
    if (settings.i2cPort->endTransmission() == 0)
        return (true);
    return (false);
}

// Returns true if device is not answering (currently writing)
// Caller can pass in an I2C address. This is helpful for larger EEPROMs that have two addresses (see block bit 2).
bool ExternalEEPROM::isBusy(uint8_t i2cAddress)
{
    if (i2cAddress == 255)
        i2cAddress = settings.deviceAddress; // We can't set the default to settings.deviceAddress so we use 255 instead

    if (isConnected(i2cAddress))
        return (false);
    return (true);
}

// void ExternalEEPROM::settings(struct_memorySettings newSettings)
//{
//   settings.deviceAddress = newSettings.deviceAddress;
// }

// Old get/setMemorySize. Use get/setMemorySizeBytes
void ExternalEEPROM::setMemorySize(uint32_t memSize)
{
    setMemorySizeBytes(memSize);
}
uint32_t ExternalEEPROM::getMemorySize()
{
    return getMemorySizeBytes();
}
void ExternalEEPROM::setMemorySizeBytes(uint32_t memSize)
{
    settings.memorySize_bytes = memSize;
}
uint32_t ExternalEEPROM::getMemorySizeBytes()
{
    return settings.memorySize_bytes;
}

// Old get/setPageSize. Use get/setPageSizeBytes
void ExternalEEPROM::setPageSize(uint16_t pageSize)
{
    setPageSizeBytes(pageSize);
}
uint16_t ExternalEEPROM::getPageSize()
{
    return getPageSizeBytes();
}
void ExternalEEPROM::setPageSizeBytes(uint16_t pageSize)
{
    settings.pageSize_bytes = pageSize;
}
uint16_t ExternalEEPROM::getPageSizeBytes()
{
    return settings.pageSize_bytes;
}

// Old get/setPageWriteTime. Use get/setPageWriteTimeMs
void ExternalEEPROM::setPageWriteTime(uint8_t writeTimeMS)
{
    setPageWriteTimeMs(writeTimeMS);
}
uint8_t ExternalEEPROM::getPageWriteTime()
{
    return getPageWriteTimeMs();
}
void ExternalEEPROM::setPageWriteTimeMs(uint8_t writeTimeMS)
{
    settings.pageWriteTime_ms = writeTimeMS;
}
uint8_t ExternalEEPROM::getPageWriteTimeMs()
{
    return settings.pageWriteTime_ms;
}

void ExternalEEPROM::enablePollForWriteComplete()
{
    settings.pollForWriteComplete = true;
}
void ExternalEEPROM::disablePollForWriteComplete()
{
    settings.pollForWriteComplete = false;
}

constexpr uint16_t ExternalEEPROM::getI2CBufferSize()
{
    return I2C_BUFFER_LENGTH_TX;
}

uint32_t ExternalEEPROM::putString(uint32_t eepromLocation, String &strToWrite)
{
    uint16_t strLen = strToWrite.length() + 1;
    write(eepromLocation, (uint8_t *)strToWrite.c_str(), strLen);
    return eepromLocation + strLen;
}
void ExternalEEPROM::getString(uint32_t eepromLocation, String &strToRead)
{
    if (strToRead.length())
    {
        strToRead.remove(0, strToRead.length());
    }
    uint8_t tmp = 65; // dummy
    while (tmp != 0)
    {
        tmp = read(eepromLocation);
        if (tmp != 0)
        {
            strToRead += static_cast<char>(tmp);
        }
        eepromLocation++;
    }
}

// Attempt write-then-reads until failure
// Start at 128 bit and build up: 256, 1024, 2k, 4k, 8k, 16k, 32k, 64k, 128k, 256k, 512k, 1M
// Identifies the following EEPROM types and their variants:
// 24LC00 - 128 bit
// 24LC01 - 1024 bit
// 24LC02 - 2048 bit
// For EEPROMs of 4k, 8k, and 16k bit, there are three bits called
// 'block select bits' inside the address byte that are used
// 24LC04 - 4096 bit / 512 bytes
// 24LC08 - 8192 bit / 1024 bytes
// 24LC16 - 16384 bit / 2048 bytes
// For 32k, 64k, 128k, 256k, and 512k bit we need two address bytes
// 24LC32 - 32768 bit / 4096 bytes
// 24LC64 - 65536 bit / 8192 bytes
// 24LC128 - 131072 bit / 16384 bytes
// 24LC256 - 262144 bit / 32768 bytes
// 24LC512 - 524288 bit / 65536 bytes
// At 1Mbit (128,000 byte) there are two address bytes and a block select bit is used
// but at the upper end of the address bits (so instead of A2/A1/A0 it's B0/A1/A0).
// 24LC1024 - 1024000 bit / 128000 byte
uint32_t ExternalEEPROM::detectMemorySizeBytes()
{
    uint32_t eepromLocation = (128 / 8) - 1; // Start at the last spot of the smallest EEPROM
    uint32_t lastGoodLocation = 0;
    uint8_t i2cAddress = settings.deviceAddress;

    // Check for 24LC04/8/16
    if (isConnected(i2cAddress | 0b001) == true)
    {
        if (isConnected(i2cAddress | 0b010) == true)
        {
            if (isConnected(i2cAddress | 0b100) == true)
            {
                return (2048); // 24LC16 - 2048 bytes
            }
            return (1024); // 24LC08 - 1024 bytes
        }
        return (512); // 24LC04 - 512 bytes
    }

    // Check for 24LC1024
    if (isConnected(i2cAddress | 0b100) == true)
        return (128000); // 24LC1024 - 128000 bytes

    // We've eliminated all the types we can. Now We have to do a read-write-read-write to test.
    while (1)
    {
        byte magicValue = random(0, 254); // Avoid 0xFF = 255. Assumes user has randomSeed()ed something.

        setMemorySizeBytes(eepromLocation + 1);

        // Read and store before we start (potentially) writing
        byte originalValue = read(eepromLocation);

        // Write beyond the edge of this block
        write(eepromLocation, magicValue); // Will use a 1 or 2 byte address depending on setMemorySizeBytes()

        // Read data
        byte found = read(eepromLocation);
        if (found != magicValue && eepromLocation < 4096) // Write failed at lower locations
        {
            // We failed to write at lower locations because this is a two address type device
            eepromLocation = 2047; // Jump to two byte addresses
        }
        else if (found != magicValue) // Write failed
        {
            // Continue to increase EEPROM location and writes
        }
        else // Write was successful
        {
            // Return spot to its original value
            write(eepromLocation, originalValue);

            lastGoodLocation = eepromLocation;
        }

        if (eepromLocation >= (65536 * 1) - 1) // Limit to 512kBit
            break;

        // If good, increase size and loop
        eepromLocation += 1; // 15 becomes 16
        eepromLocation *= 2; // 16 becomes 32
        eepromLocation -= 1; // 31 becomes 31
    }

    return (lastGoodLocation + 1);
}

// Read a byte from a given location
uint8_t ExternalEEPROM::read(uint32_t eepromLocation)
{
    uint8_t tempByte;
    read(eepromLocation, &tempByte, 1);
    return tempByte;
}

// Bulk read from EEPROM
// Handles breaking up read amt into 32 byte chunks (can be overriden with setI2CBufferSize)
// Handles a read that straddles the 512kbit barrier
int ExternalEEPROM::read(uint32_t eepromLocation, uint8_t *buff, uint16_t bufferSize)
{
    int result = 0;

    uint16_t received = 0;
    while (received < bufferSize)
    {
        // Limit the amount to write to a page size
        uint16_t amtToRead = bufferSize - received;
        if (amtToRead > I2C_BUFFER_LENGTH_RX) // Arduino I2C buffer size limit
            amtToRead = I2C_BUFFER_LENGTH_RX;

        // Check if we are dealing with large (>512kbit) EEPROMs
        uint8_t i2cAddress = settings.deviceAddress;
        if (settings.memorySize_bytes > 0xFFFF)
        {
            // Figure out if we are going to cross the barrier with this read
            if (eepromLocation + received < 0xFFFF)
            {
                if (0xFFFF - (eepromLocation + received) < amtToRead) // 0xFFFF - 0xFFFA < 32
                    amtToRead =
                        0xFFFF - (eepromLocation + received); // Limit the read amt to go right up to edge of barrier
            }

            // Figure out if we are accessing the lower half or the upper half
            if (eepromLocation + received > 0xFFFF)
                i2cAddress |= 0b100; // Set the block bit to 1
        }
        // See if EEPROM is available or still writing a previous request
        while (settings.pollForWriteComplete && isBusy(i2cAddress) == true) // Poll device
            delayMicroseconds(100);                                         // This shortens the amount of time waiting between writes but hammers the I2C bus

        settings.i2cPort->beginTransmission(i2cAddress);
        if (getMemorySizeBytes() > 2048)
            settings.i2cPort->write((uint8_t)((eepromLocation + received) >> 8)); // MSB
        settings.i2cPort->write((uint8_t)((eepromLocation + received) & 0xFF));   // LSB

        result = settings.i2cPort->endTransmission();

        settings.i2cPort->requestFrom((uint8_t)i2cAddress, (size_t)amtToRead);

        for (uint16_t x = 0; x < amtToRead; x++)
            buff[received + x] = settings.i2cPort->read();

        received += amtToRead;
    }

    return (result);
}

// Write a byte to a given location
int ExternalEEPROM::write(uint32_t eepromLocation, uint8_t dataToWrite)
{
    if (read(eepromLocation) != dataToWrite) // Update only if data is new
        return (write(eepromLocation, &dataToWrite, 1));
    return (0);
}

// Write large bulk amounts
// Limits writes to the I2C buffer size (default is 32 bytes)
// Returns the result of the I2C endTransmission
int ExternalEEPROM::write(uint32_t eepromLocation, const uint8_t *dataToWrite, uint16_t bufferSize)
{
    int result = 0;

    // Error check
    if (eepromLocation + bufferSize >= settings.memorySize_bytes)
        bufferSize = settings.memorySize_bytes - eepromLocation;

    int16_t maxWriteSize = settings.pageSize_bytes;
    if (maxWriteSize > I2C_BUFFER_LENGTH_TX - 2)
        maxWriteSize = I2C_BUFFER_LENGTH_TX - 2; // Arduino has 32 byte limit. We loose two to the EEPROM address

    // Break the buffer into page sized chunks
    uint16_t recorded = 0;
    while (recorded < bufferSize)
    {
        // Limit the amount to write to either the page size or the Arduino limit of 30
        int amtToWrite = bufferSize - recorded;
        if (amtToWrite > maxWriteSize)
            amtToWrite = maxWriteSize;

        if (amtToWrite > 1)
        {
            // Check for crossing of a page line. Writes cannot cross a page line.
            uint16_t pageNumber1 = (eepromLocation + recorded) / settings.pageSize_bytes;
            uint16_t pageNumber2 = (eepromLocation + recorded + amtToWrite - 1) / settings.pageSize_bytes;
            if (pageNumber2 > pageNumber1)
                amtToWrite = ((pageNumber1 + 1) * settings.pageSize_bytes) -
                             (eepromLocation + recorded); // Limit the write amt to go right up to edge of page barrier
        }

        uint8_t i2cAddress = settings.deviceAddress;
        // Check if we are dealing with large (>512kbit) EEPROMs
        if (settings.memorySize_bytes > 0xFFFF)
        {
            // Figure out if we are accessing the lower half or the upper half
            if (eepromLocation + recorded > 0xFFFF)
                i2cAddress |= 0b100; // Set the block bit to 1
        }

        // See if EEPROM is available or still writing a previous request
        while (settings.pollForWriteComplete && isBusy(i2cAddress) == true) // Poll device
            delayMicroseconds(100);                                         // This shortens the amount of time waiting between writes but hammers the I2C bus

        settings.i2cPort->beginTransmission(i2cAddress);
        if (getMemorySizeBytes() > 2048)                                          // Device larger than 16,384 bits have two byte addresses
            settings.i2cPort->write((uint8_t)((eepromLocation + recorded) >> 8)); // MSB
        settings.i2cPort->write((uint8_t)((eepromLocation + recorded) & 0xFF));   // LSB

        for (uint8_t x = 0; x < amtToWrite; x++)
            settings.i2cPort->write(dataToWrite[recorded + x]);

        result = settings.i2cPort->endTransmission(); // Send stop condition

        recorded += amtToWrite;

        if (settings.pollForWriteComplete == false)
            delay(settings.pageWriteTime_ms); // Delay the amount of time to record a page
    }

    return (result);
}
