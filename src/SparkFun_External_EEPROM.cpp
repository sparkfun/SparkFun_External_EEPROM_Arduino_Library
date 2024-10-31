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

#include "SparkFun_External_EEPROM.h"
#include "Arduino.h"
#include "Wire.h"

bool ExternalEEPROM::begin(uint8_t deviceAddress, TwoWire &wirePort, uint8_t WP)
{
    if(WP != 255)
    {
        pinMode(WP, OUTPUT);
        digitalWrite(WP, HIGH);
        settings.wpPin = WP;
    }
    settings.i2cPort = &wirePort; // Grab which port the user wants us to use
    settings.deviceAddress = deviceAddress;

    if (isConnected() == false)
    {
        return false;
    }

    // if (settings.addressSize_bytes == 0)
    // {
    //     detectAddressBytes();
    //     // Serial.print("Detected number of address bytes: ");
    //     // Serial.println(getAddressBytes());
    // }

    // if (settings.pageSize_bytes == 0)
    // {
    //     detectPageSizeBytes();
    //     // Serial.print("Detected page size: ");
    //     // Serial.println(getPageSizeBytes());
    // }

    // if (settings.writeTime_ms == 0)
    // {
    //     detectWriteTimeMs();
    //     Serial.print("Detected write time (ms): ");
    //     Serial.println(getWriteTimeMs());
    // }

    // if (settings.memorySize_bytes == 0)
    // {
    //     detectMemorySizeBytes();
    //     // Serial.print("Detected memory size: ");
    //     // Serial.println(getMemorySizeBytes());
    // }

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

    //Try to identify this memory size settings
    switch (memSize)
    {
    default:
        // Unknown memory size
        break;
    case (16):
        setAddressBytes(1);
        setPageSizeBytes(1);
        break;
    case (128):
        setAddressBytes(1);
        setPageSizeBytes(8);
        break;
    case (256):
        setAddressBytes(1);
        setPageSizeBytes(8);
        break;
    case (512):
        setAddressBytes(1);
        setPageSizeBytes(16);
        break;
    case (1024):
        setAddressBytes(1);
        setPageSizeBytes(16);
        break;
    case (2048):
        setAddressBytes(1);
        setPageSizeBytes(16);
        break;
    case (4096):
        setAddressBytes(2);
        setPageSizeBytes(32);
        break;
    case (8192):
        setAddressBytes(2);
        setPageSizeBytes(32);
        break;
    case (16384):
        setAddressBytes(2);
        setPageSizeBytes(64);
        break;
    case (32768):
        setAddressBytes(2);
        setPageSizeBytes(64);
        break;
    case (65536):
        setAddressBytes(2);
        setPageSizeBytes(128);
        break;
    case (128000):
        setAddressBytes(2);
        setPageSizeBytes(128);
        break;
    case (262144):
        setAddressBytes(2);
        setPageSizeBytes(256);
        break;
    }
}
uint32_t ExternalEEPROM::getMemorySizeBytes()
{
    return settings.memorySize_bytes;
}

void ExternalEEPROM::setMemoryType(uint16_t typeNumber)
{
    //Set settings based on known memory types
    switch (typeNumber)
    {
    default:
        // Unknown type number
        break;
    case (0):
        setMemorySizeBytes(16);
        break;
    case (1):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //128
        break;
    case (2):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //256
        break;
    case (4):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //512
        break;
    case (8):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //1024
        break;
    case (16):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //2048
        break;
    case (32):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //4096
        break;
    case (64):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //8192
        break;
    case (128):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //16384
        break;
    case (256):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //32768
        break;
    case (512):
        setMemorySizeBytes(128 * (uint32_t)typeNumber); //65536
        break;
    case (1025):
        setMemorySizeBytes(128000); //128000
        break;
    case (2048):
        setMemorySizeBytes(262144); //262144
        break;
    }
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

// Old get/setPageWriteTime. Use get/setWriteTimeMs
void ExternalEEPROM::setPageWriteTime(uint8_t writeTimeMS)
{
    setWriteTimeMs(writeTimeMS);
}
uint8_t ExternalEEPROM::getPageWriteTime()
{
    return getWriteTimeMs();
}
void ExternalEEPROM::setWriteTimeMs(uint8_t writeTimeMS)
{
    settings.writeTime_ms = writeTimeMS;
}
uint8_t ExternalEEPROM::getWriteTimeMs()
{
    return settings.writeTime_ms;
}

// Write a value to EEPROM, takes an average of time taken
uint8_t ExternalEEPROM::detectWriteTimeMs(uint8_t numberOfTests)
{
    uint8_t testLocation = 5; // Location in memory to do our tests

    uint8_t originalValue = read(testLocation); // Preserve data before we start writing

    uint32_t totalTime = 0;
    const uint8_t percentOverage = 10;

    // Create copy of internal settings before test
    uint32_t originalMemorySize = settings.memorySize_bytes;
    bool originalpollForWriteComplete = settings.pollForWriteComplete;

    setMemorySizeBytes(128); // Assume the smallest memory size during test
    settings.pollForWriteComplete = true;

    // We can't run this test if we don't know the number of address bytes
    if (settings.addressSize_bytes == 0)
        detectAddressBytes();

    for (int x = 0; x < numberOfTests; x++)
    {
        // Avoid the default state of 0xFF = 255 and 0. Assumes user has randomSeed()ed something.
        // Do not use the original value
        // Do not use the value found in the next location either
        uint8_t magicValue = 0;
        do
        {
            magicValue = random(1, 255); // (Inclusive, exclusive)
        } while (magicValue == originalValue);

        unsigned long startTime = micros();
        write(testLocation, magicValue); // Does a read before write. Uses polling.

        // Wait until write completes
        while (isBusy(settings.deviceAddress) == true) // Poll device's original address, not the modified one
            delayMicroseconds(100); // This shortens the amount of time waiting between writes but hammers the I2C bus

        unsigned long stopTime = micros();
        totalTime += (stopTime - startTime);

        // Serial.print("delta: ");
        // Serial.println((stopTime - startTime));
    }

    //  Return spot to its original value
    write(testLocation, originalValue);

    // Return original settings
    settings.memorySize_bytes = originalMemorySize;
    settings.pollForWriteComplete = originalpollForWriteComplete;

    uint16_t avgTimeUs = totalTime / numberOfTests;

    // Serial.print("avgTimeUs: ");
    // Serial.println(avgTimeUs);

    settings.writeTime_ms =
        ceil(avgTimeUs * ((1.0 + percentOverage / 100.0) / 1000.0)); // Apply overage, convert to ms, round up

    return (settings.writeTime_ms);
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
    return (eepromLocation + strLen);
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

// Assuming there are discrete EEPROM sizes, return the number of bytes in the next model
uint32_t getNextSizeBytes(uint32_t currentSizeBytes)
{
    if (currentSizeBytes == 16)
        return 128;                // 24LC00 - Lowest value
    return (currentSizeBytes * 2); // 24LC01, 02, 04, 08, etc
}

void ExternalEEPROM::setAddressBytes(uint8_t addressBytes)
{
    settings.addressSize_bytes = addressBytes;
}
uint8_t ExternalEEPROM::getAddressBytes()
{
    return settings.addressSize_bytes;
}

// Determines the number of address bytes to complete a successful write
// Returns 1 or 2
// Sets the internal setting
uint8_t ExternalEEPROM::detectAddressBytes()
{
    uint8_t testLocation = 1;

    // Create copy of internal settings before test
    uint32_t originalMemorySize = settings.memorySize_bytes;
    uint16_t originalPageSize = settings.pageSize_bytes;

    // Read and store before we start (potentially) writing
    // This will fail on two byte address EEPROMs when the memory size is below 4096 bytes
    uint8_t locationValueOriginal = read(testLocation);

    // Serial.print("locationValueOriginal: 0x");
    // Serial.print(locationValueOriginal, HEX);

    setMemorySizeBytes(128); // Assume the smallest memory size during test
    setPageSizeBytes(1);     // Assume a page size

    uint8_t addressBytes = 1;
    for (; addressBytes < 3; addressBytes++)
    {
        setAddressBytes(addressBytes); // Start test at one byte

        // Avoid the default state of 0xFF = 255 and 0. Assumes user has randomSeed()ed something.
        // Do not use the original value
        // Do not use the value found in the next location either
        uint8_t magicValue = 0;
        do
        {
            magicValue = random(1, 255); // (Inclusive, exclusive)
        } while (magicValue == locationValueOriginal);

        // Serial.print(" writing: 0x");
        // Serial.print(magicValue, HEX);

        // Write this new value
        write(testLocation, magicValue); // Will use a 1 or 2 byte address depending on setMemorySizeBytes()

        // Read data back
        uint8_t locationValue = read(testLocation);
        // Serial.print(" read: 0x");
        // Serial.print(locationValue, HEX);
        // Serial.print(" - ");

        if (locationValue == magicValue)
        {
            // Successful write. We've determined the number of address bytes
            break;
        }
    }

    // Serial.println("\r\nWrite success. One byte addressing.");
    //  Return spot to its original value
    write(testLocation, locationValueOriginal);

    // Return original settings
    settings.memorySize_bytes = originalMemorySize;
    settings.pageSize_bytes = originalPageSize;

    // Error check
    if (addressBytes >= 3)
        addressBytes = 1; // If we failed, 1 guarantees we won't corrupt data with two byte address writes

    settings.addressSize_bytes = addressBytes;
    return (settings.addressSize_bytes);
}

// Determine the number of bytes we can write in a single go
// Valid amounts are 1, 8, 16, 32, 128, and 256 bytes
// Sets the internal setting
uint16_t ExternalEEPROM::detectPageSizeBytes()
{
#define maxPageSize 256 // Used in very large 2M bit EEPROMs

    uint8_t originalValuesArray[maxPageSize]; // Preserves data in EEPROM before we start writing
    uint8_t tempArray[maxPageSize];
    uint8_t testLocation = 0; // Location in memory to do our tests

    // We can't run this test if we don't know the number of address bytes
    if (settings.addressSize_bytes == 0)
        detectAddressBytes();

    // We can't run this test unless we know how to properly read/write to the device
    if (settings.memorySize_bytes == 0)
        detectMemorySizeBytes();

    // Read chunk from EEPROM and store
    for (uint16_t x = 0; x < maxPageSize; x++)
        originalValuesArray[x] = read(testLocation + x); // Read byte wise to avoid page size limitations

    uint16_t pageSizeBytes = 8;
    bool detectedPageSize = false;
    while (1)
    {
        // Create new array based on the original values
        for (uint16_t x = 0; x < pageSizeBytes; x++)
            tempArray[x] = (uint8_t)(originalValuesArray[x] + x); // Avoid original values

        setPageSizeBytes(pageSizeBytes); // Tell library to write this number of bytes at a time

        // Write new magic values to EEPROM
        write(testLocation, tempArray, pageSizeBytes);

        // Read magic values from EEPROM
        read(testLocation, tempArray, pageSizeBytes);

        // Check values
        for (uint16_t x = 0; x < pageSizeBytes; x++)
        {
            // Serial.print("temp: ");
            // Serial.print(tempArray[x]);
            // Serial.print(" original: ");
            // Serial.print((uint8_t)(originalValuesArray[x] + x));
            // Serial.println();

            if (tempArray[x] != (uint8_t)(originalValuesArray[x] + x))
            {
                // We've detected the limit

                // Serial.print("pageSize: ");
                // Serial.print(pageSizeBytes);
                // Serial.print(" write failed. Previous page size is the answer ");

                // Reduce the page size to the last value
                if (pageSizeBytes == 32 || pageSizeBytes == 16 || pageSizeBytes == 256)
                    pageSizeBytes /= 2;
                else if (pageSizeBytes == 128)
                    pageSizeBytes = 32;
                else if (pageSizeBytes == 8)
                    pageSizeBytes = 1;

                detectedPageSize = true;

                break;
            }
        }

        if (detectedPageSize)
            break;

        // Serial.print("pageSizeBytes: ");
        // Serial.print(pageSizeBytes);
        // Serial.println(" passed");

        // Go to next page size
        if (pageSizeBytes == 8 || pageSizeBytes == 16 || pageSizeBytes == 128)
            pageSizeBytes *= 2;
        else if (pageSizeBytes == 32)
            pageSizeBytes = 128;
        else if (pageSizeBytes == maxPageSize)
            break; // EEPROMs with larger than 256 byte page writes are not known at this time.

        // We can't write more than I2C_BUFFER_LENGTH_TX at a time so that is the limit of our pageSize testing.
        if (pageSizeBytes > I2C_BUFFER_LENGTH_TX)
        {
            // Serial.print("Page size test limited by platform I2C buffer of: ");
            // Serial.println(I2C_BUFFER_LENGTH_TX);
            break;
        }
    }

    settings.pageSize_bytes = pageSizeBytes;

    // Write original chunk to EEPROM
    write(testLocation, originalValuesArray, pageSizeBytes);

    return (settings.pageSize_bytes);
}

// Attempts write-then-reads until failure to determine memory bounds
// Identifies the following EEPROM types and their variants:
// 24LC00 - 128 bit / 16 bytes - 1 address byte, 1 byte page size
// 24LC01 - 1024 bit / 128 bytes - 1 address byte, 8 byte page size
// 24LC02 - 2048 bit / 256 bytes - 1 address byte, 8 byte page size
// 24LC04 - 4096 bit / 512 bytes - 1 address byte, 16 byte page size
// 24LC08 - 8192 bit / 1024 bytes - 1 address byte, 16 byte page size
// 24LC16 - 16384 bit / 2048 bytes - 1 address byte, 16 byte page size
// 24LC32 - 32768 bit / 4096 bytes - 2 address bytes, 32 byte page size
// 24LC64 - 65536 bit / 8192 bytes - 2 address bytes, 32 byte page size
// 24LC128 - 131072 bit / 16384 bytes - 2 address bytes, 64 byte page size
// 24LC256 - 262144 bit / 32768 bytes - 2 address bytes, 64 byte page size
// 24LC512 - 524288 bit / 65536 bytes - 2 address bytes, 128 byte page size
// 24LC1024 - 1024000 bit / 128000 byte - 2 address bytes, 128 byte page size
// 24CM02 - 2097152 bit / 262144 byte - 2 address bytes, 256 byte page size
// For EEPROMs of 4k, 8k, and 16k bit, there are three bits called
// 'block select bits' inside the address byte that are used
// For 32k, 64k, 128k, 256k, and 512k bit we need two address bytes
// At 1Mbit (128,000 byte) and above there are two address bytes and a block select bit
// is used but at the upper end of the address bits (so instead of A2/A1/A0 it's B0/A1/A0).
uint32_t ExternalEEPROM::detectMemorySizeBytes()
{
    // We do a read-write-read-write to test.
    uint32_t testLocation = (128 / 8) - 1; // Start at the last spot of the smallest EEPROM
    uint32_t lastGoodLocation = 0;

    // We can't run this test if we don't know the number of address bytes
    if (settings.addressSize_bytes == 0)
        detectAddressBytes();

    // detectPageSizeBytes() calls this function so we cannot call it (endless loop)
    // Because detectMemorySizeBytes() is only doing single byte writes, a page size of 1 is fine for now
    if (settings.pageSize_bytes == 0)
        settings.pageSize_bytes = 1;

    // Determine if we are dealing with one or two byte addressing
    uint8_t addressBytes = getAddressBytes();
    if (addressBytes == 2)
        testLocation = 4096 - 1; // Start test with 2-byte addresses
    else
        testLocation = 16 - 1; // Start test with 1-byte addresses

    // Smaller EEPROMs ignore and set the address bits A7 through A10 to zero. This causes writes to wrap around.
    // So to detect the edge of the usable space, we write to a location beyond the known edge of memory.
    // If that same value is at the last known good edge of memory then we know the address is ignoring the address
    // bit, it has wrapped the address, and we can infer the edge of memory.

    // 1 Read current value to preserve it
    // 2 Read current value at next threshold
    // 3 Write value to current spot
    // 4 Check value at next threshold. If it was changed then we know the address we wrote was wrapped.
    // 5 Return memory spot to its original value

    while (1)
    {
        // We must set our memory size to the next size up so that write sets the correct bits
        uint32_t nextLocation = getNextSizeBytes(testLocation + 1) - 1;

        // Serial.print("nextLocation: ");
        // Serial.print(nextLocation);

        setMemorySizeBytes(nextLocation + 1);

        // Serial.print(" Test: ");
        // Serial.print(nextLocation + 1);
        // Serial.print(" bytes/");
        // Serial.print((nextLocation + 1) * 8);
        // Serial.print(" bits");
        // Serial.print(", ");

        // Read and store before we start (potentially) writing
        uint8_t originalValue = read(testLocation);

        // Serial.print("originalValue: 0x");
        // Serial.print(originalValue, HEX);

        uint8_t nextLocationOriginalValue = read(nextLocation);

        // Serial.print(" nextLocationOriginalValue: 0x");
        // Serial.print(nextLocationOriginalValue, HEX);
        // Serial.print(" - ");

        // Avoid the default state of 0xFF = 255 and 0. Assumes user has randomSeed()ed something.
        // Do not use the original value
        // Do not use the value found in the next location either
        uint8_t magicValue = random(1, 255); // (Inclusive, exclusive)
        while (magicValue == originalValue && magicValue == nextLocationOriginalValue)
            magicValue = random(1, 255); // (Inclusive, exclusive)

        // Serial.print("writing: 0x");
        // Serial.print(magicValue, HEX);

        // Write this new value to the edge of this block
        write(testLocation, magicValue); // Will use a 1 or 2 byte address depending on setMemorySizeBytes()

        // Read back data
        uint8_t newValue = read(testLocation);

        // Serial.print(" read: 0x");
        // Serial.print(newValue, HEX);
        // Serial.print(" - ");

        // Read data from the next spot before we return the original spot value
        uint8_t nextNewValue = read(nextLocation);

        // Serial.print("nextNewValue: 0x");
        // Serial.print(nextNewValue, HEX);
        // Serial.print(" - ");

        // Serial.println();

        // Return spot to its original value
        write(testLocation, originalValue);

        lastGoodLocation = testLocation;

        // We should be able to write to all locations. But bail if we fail.
        if (newValue != magicValue)
        {
            // Serial.println("\r\nWrite failed");
            break;
        }

        // We assume the address at the end of this memory space has changed,
        // but if the address at the next level *also* changes, then we know the
        // memory address has wrapped because the IC zeroes out the extra bits
        else if (nextNewValue == magicValue)
        {
            // Serial.println("The write affected the next level - we're done!");
            break;
        }

        if (testLocation >= (65536 * 2) - 1) // Limit to 512kBit
            break;

        // Move to next location
        testLocation = nextLocation;
    }

    settings.memorySize_bytes = lastGoodLocation + 1;

    // Serial.print("Memory size in bytes: ");
    // Serial.println(settings.memorySize_bytes);

    return (settings.memorySize_bytes);
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
        // Check if we are dealing with 24LC04/08/16 (512, 1024, and 2048 bytes)
        // These use a single address byte but change the I2C address
        else if (settings.memorySize_bytes >= 512 && settings.memorySize_bytes <= 2048)
        {
            // Set I2C Address bits (A2/A1/A0) accordingly
            i2cAddress |= ((eepromLocation + received) >> 8);
        }

        if (settings.pollForWriteComplete == false)
        {
            delay(settings.writeTime_ms);
        }

        // See if EEPROM is available or still writing a previous request
        while (isBusy(settings.deviceAddress) == true) // Poll device's original address, not the modified one
            delayMicroseconds(100); // This shortens the amount of time waiting between writes but hammers the I2C bus

        settings.i2cPort->beginTransmission(i2cAddress);
        if (settings.addressSize_bytes > 1)
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

    // Serial.print("bufferSize: ");
    // Serial.println(bufferSize);

    int16_t maxWriteSize = settings.pageSize_bytes;
    if (maxWriteSize > I2C_BUFFER_LENGTH_TX - settings.addressSize_bytes)
        maxWriteSize =
            I2C_BUFFER_LENGTH_TX -
            settings.addressSize_bytes; // Arduino has 32 byte limit. We loose 1 or 2 bytes to the EEPROM address

    // Serial.print("maxWriteSize: ");
    // Serial.println(maxWriteSize);

    // Break the buffer into page sized chunks
    uint16_t recorded = 0;
    while (recorded < bufferSize)
    {
        // Limit the amount to write to either the page size or the Arduino limit of 30
        int amtToWrite = bufferSize - recorded;

        // Serial.print("amtToWrite: ");
        // Serial.println(amtToWrite);

        if (amtToWrite > maxWriteSize)
            amtToWrite = maxWriteSize;

        // Serial.print("amtToWrite: ");
        // Serial.println(amtToWrite);

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
        // These use two address bytes and a B0 'block' bit
        if (settings.memorySize_bytes > 0xFFFF)
        {
            // Figure out if we are accessing the lower half or the upper half
            if (eepromLocation + recorded > 0xFFFF)
                i2cAddress |= 0b100; // Set the block bit to 1
        }
        // Check if we are dealing with 24LC04/08/16 (512, 1024, and 2048 bytes)
        // These use a single address byte but change the I2C address
        else if (settings.memorySize_bytes >= 512 && settings.memorySize_bytes <= 2048)
        {
            // Set I2C Address bits (A2/A1/A0) accordingly
            i2cAddress |= ((eepromLocation + recorded) >> 8);
        }

        if (settings.pollForWriteComplete == false)
        {
            delay(settings.writeTime_ms);
        }

        // See if EEPROM is available or still writing a previous request
        while (isBusy(settings.deviceAddress) == true) // Poll device's original address, not the modified one
            delayMicroseconds(100); // This shortens the amount of time waiting between writes but hammers the I2C bus

        // Check if we are using Write Protection then disable WP for write access
        if(settings.wpPin != 255 ) digitalWrite(settings.wpPin, LOW);

        settings.i2cPort->beginTransmission(i2cAddress);
        if (settings.addressSize_bytes > 1) // Device larger than 16,384 bits have two byte addresses
            settings.i2cPort->write((uint8_t)((eepromLocation + recorded) >> 8)); // MSB
        settings.i2cPort->write((uint8_t)((eepromLocation + recorded) & 0xFF));   // LSB

        for (uint16_t x = 0; x < amtToWrite; x++)
            settings.i2cPort->write(dataToWrite[recorded + x]);

        result = settings.i2cPort->endTransmission(); // Send stop condition

        recorded += amtToWrite;

        // Serial.print("recorded: ");
        // Serial.println(recorded);

        if (settings.pollForWriteComplete == false)
            delay(settings.writeTime_ms); // Delay the amount of time to record a page

        // Enable Write Protection if we are using WP
        if(settings.wpPin != 255) digitalWrite(settings.wpPin, HIGH);
    }

    return (result);
}
