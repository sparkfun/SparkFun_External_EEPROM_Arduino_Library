SparkFun External EEPROM Arduino Library
===========================================================

![SparkFun Qwiic EEPROM](https://cdn.sparkfun.com//assets/parts/1/3/0/0/8/14764-Qwiic_EEPROM_-_512Kbit-01.jpg)

[*SparkFun Qwiic EEPROM (SPX-14764)*](https://www.sparkfun.com/products/14764)

A simple to use I2C library for talking to any EEPROM. It uses the same template system found in the Arduino EEPROM library so you can use the same get() and put() functions.

Various external EEPROMs have various interface specs (overall size, page size, write times, etc). This library works with all types and allows the various settings to be set at runtime. All read and write restrictions associated with pages are taken care of. You can access the external memory as if it was contiguous.

Best used with the Qwiic EEPROM: https://www.sparkfun.com/products/14764

This library can be installed via the Arduino Library manager. Search for **SparkFun External EEPROM**.

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
