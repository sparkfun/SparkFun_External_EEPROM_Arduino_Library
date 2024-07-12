SparkFun External EEPROM Arduino Library
===========================================================

![SparkFun Qwiic EEPROM](https://cdn.sparkfun.com//assets/parts/1/7/7/0/1/18355-SparkFun_Qwiic_EEPROM_Breakout_-_512Kbit-01.jpg)

[*SparkFun Qwiic EEPROM (COM-18355)*](https://www.sparkfun.com/products/18355)

A simple-to-use I2C library for talking to any EEPROM. It uses the same template system found in the Arduino EEPROM library so you can use the same get() and put() functions.

Various external EEPROMs have various interface specs (overall size, page size, write times, etc). This library works with all types and allows the various settings to be set at runtime. All read and write restrictions associated with pages are taken care of. You can access the external memory as if it was contiguous.

Once the library has been started, the memory type needs to be set, the following is an example for the [Qwiic 24xx512 EEPROM](https://www.sparkfun.com/products/18355):

    myMem.setMemoryType(512); 

Where *512* is the model (ie, 24LC**512**). Setting the memory type configures the memory size in bytes, the number of address bytes, and the page size in bytes. The following memory types are valid: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1025, 2048

* **0** - 24xx00 / ie [24LC00](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC00%20-%20128.pdf)
* **1** - 24xx01 / ie [24LC01B](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC01%20-%201k.pdf)
* **2** - 24xx02 / ie [24LC02B](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC02%20-%202k.pdf)
* **4** - 24xx04 / ie [CAT24C04](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC04%20-%204k%20Onsemi.PDF)
* **8** - 24xx08 / ie [BR24G08](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC08%20-%208k%20Rohm.pdf)
* **16** - 24xx16 / ie [24AA16](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC16%20-%2016k.pdf)
* **32** - 24xx32 / ie [24LC32A](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC32%20-%2032k.pdf)
* **64** - 24xx64 / ie [24FC64](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC64%20-%2064k.pdf)
* **128** - 24xx128 / ie [24LC128](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC128-%20128k.pdf)
* **256** - 24xx256 / ie [24AA256](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC256%20-%20256k.pdf)
* **512** - 24xx512 / ie [24C512C](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC512%20-%20512k.pdf)
* **1025** - 24xx1025 / ie [24LC1025](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC1024%20-%201Mbit.pdf)
* **2048** - 24xx2048 / ie [AT24CM02](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs/blob/main/24LC2048%20-%202Mbit.pdf)

For a list of all the EEPROM datasheets, please see [this repo](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library_Docs). We don't want to store the PDFs in the library repo, otherwise, every user will have to download all the PDFs just to install the library.

Alternatively, the individual settings can be set. If setMemorySizeBytes/setAddressBytes/setPageSizeBytes() are called, they will overwrite any previous settings set by `setMemoryType()`.

    myMem.setMemorySizeBytes(65536);
    myMem.setAddressBytes(2); // Set address bytes and page size after MemorySizeBytes()
    myMem.setPageSizeBytes(128);

Set the memory type, or set the memory settings, but not both.

This library is best used with the [Qwiic EEPROM](https://www.sparkfun.com/products/18355).

This library can be installed via the Arduino Library manager. Search for **SparkFun External EEPROM**.

Want to help? Please do! We are always looking for ways to improve and build out features of this library.

Thanks to:

* [Hutch67](https://github.com/Hutch67) for correcting the [test for pollForWriteComplete setting](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/4/files)
* [TylerBird](https://github.com/TylerBird) for correcting [ambiguous int declaration](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/7) and adding STM32 support
* [quarcko](https://github.com/quarcko) for [small EEPROM support](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/11) (24LC02).
* [robsonos](https://github.com/robsonos) adding for [nRF52 support](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/12).
* [RayNieport](https://github.com/RayNieport) for adding [RP2040 support](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/14).
* [wolfbert](https://github.com/wolfbert) for adding [ESP8266 support](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/15).
* [romain145](https://github.com/romain145) for fixing a [multi-page write](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/18) bug.
* [wollewald](https://github.com/wollewald) for read/write strings.
* [giminotron5](https://github.com/giminotron5) for adding [write protect control](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/30).
* [merlinz01](https://github.com/merlinz01) for adding [putChanged()](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library/pull/38) method

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
