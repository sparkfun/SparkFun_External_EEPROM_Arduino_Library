SparkFun External EEPROM Arduino Library
===========================================================

![SparkFun Qwiic EEPROM](https://cdn.sparkfun.com//assets/parts/1/7/7/0/1/18355-SparkFun_Qwiic_EEPROM_Breakout_-_512Kbit-01.jpg)

[*SparkFun Qwiic EEPROM (COM-18355)*](https://www.sparkfun.com/products/18355)

A simple-to-use I2C library for talking to any EEPROM. It uses the same template system found in the Arduino EEPROM library so you can use the same get() and put() functions.

The device-specific specs (overall size, page size, write times, etc) are auto-detected. This library works with all types and allows the various settings to be set at runtime. All read and write restrictions associated with pages are taken care of. You can access the external memory as if it was contiguous.

Best used with the Qwiic EEPROM: https://www.sparkfun.com/products/18355

This library can be installed via the Arduino Library manager. Search for **SparkFun External EEPROM**.

The method for autodetection is as follows. The original data on the EEPROM is maintained once autodetection is complete:

* Address Bytes: Smaller EEPROMs use one address byte and larger EEPROMs use 2 address bytes. The auto-detection routine attempts to read a byte and then write a new byte. If the read/write fails, the address byte count is increased and a write attempt is tried again. Once the correct number of address bytes is detected, the original value is written back to EEPROM.

* Page Size: 256 bytes of EEPROM are read individually (to avoid page reads). Then, new bytes are generated that are random *and* different from the original data. The new bytes are written to EEPROM in increasing page size tests (8 bytes, then 16, etc) until the data does not match. Then the original data is written back to the EEPROM. Note: The page size may be reported as 32 bytes when the actual page size is larger. This is because of the size of the I2C buffer available on your microcontroller's Arduino core's Wire implementation. On some platforms (such as Arduino Uno) the I2C write buffer is 32 bytes. Once the two write bytes are removed, that leaves only 30 bytes available for sequential reads/writes. For maximum write speeds, use a platform that has a larger buffer than the page size of the EEPROM you are using.

* Memory Size: Once address bytes and page size are determined, single bytes are read, then written, then checked at the end of the potential EEPROM space. For example, byte 65535 is read for its original value and stored. Then a new, different value is written to location 65535. If the read/write fails, we know the EEPROM is one size down (32768 bytes, aka 24XX256).

* Page write time: This setting is not detected at begin(). All EEPROMs have a max write time of 5ms. Testing with a logic analyzer has shown that all EEPROMs respect this max and have an average of ~4.5ms for a page write. Attempting to trim the page write time lower than 5ms is much less worthwhile than using up a properly sized page size setting with an adequately sized I2C write buffer. In other words, read and write as many bytes as the page size can handle, don't worry about setting the page write time to 4ms or 5ms. Note: The 24XXM02 has a 10ms max page write time.

Want to help? Please do! We are always looking for ways to improve and build out features of this library.

Thanks to:

* Hutch67 for correcting the [test for pollForWriteComplete setting](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/4/files)
* [TinaJ_kay](https://github.com/TylerBird) for correcting [ambiguous int declaration](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/7) and adding STM32 support

Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE. 
* **library.properties** - General library properties for the Arduino package manager. 

Documentation
--------------

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.

License Information
-------------------

This product is _**open source**_! 

Various bits of the code have different licenses applied. Anything SparkFun wrote is beerware; if you see me (or any other SparkFun employee) at the local, and you've found our code helpful, please buy us a round!

Please use, reuse, and modify these files as you see fit. Please maintain attribution to SparkFun Electronics and release anything derivative under the same license.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.
