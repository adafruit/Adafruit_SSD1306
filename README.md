# Adafruit_SSD1306 [![Build Status](https://travis-ci.org/adafruit/Adafruit_SSD1306.svg?branch=master)](https://travis-ci.org/adafruit/Adafruit_SSD1306)

This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use I2C or SPI to communicate, 2 to 5 pins are required to interface.

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. Scrolling code contributed by Michael Gregg. Dynamic buffer allocation based on work by Andrew Canaday.
BSD license, check license.txt for more information. All text above must be included in any redistribution

Preferred installation method is to use the Arduino IDE Library Manager. To download the source from Github instead, click "Clone or download" above, then "Download ZIP." After uncompressing, rename the resulting folder Adafruit_SSD1306. Check that the Adafruit_SSD1306 folder contains Adafruit_SSD1306.cpp and Adafruit_SSD1306.h.

You will also have to install the **Adafruit GFX library** which provides graphics primitves such as lines, circles, text, etc. This also can be found in the Arduino Library Manager, or you can get the source from https://github.com/adafruit/Adafruit-GFX-Library

## Changes

Version 1.2 (November 2018) introduces some significant changes:

  * Display dimensions are now specified in the constructor...you no longer need to edit the .h file for different screens (though old sketches can continue to work that way).
  * SPI transactions are used and SPI bitrate can be specified (both require Arduino 1.6 or later).
  * SPI and Wire (I2C) interfaces other than the defaults are supported.

Version 1.10 (2019) introduces another significant changes:

  * All hadrware communication moved to a driver class, while main Adafruit_SSD1306 class focuses on the display logic. This significantly improves internal library architecture, and reduces memory footprint as unused interfaces do not compile. 
    Thus if the display is connected via SPI, the I2C related code is not even compiled into the program. 
  * The change requires minor adjustments in the user sketch as follows

Prior the change:
```
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
...
display.begin(SSD1306_SWITCHCAPVCC, 0x3D)
```

Now:
```
#include <ssd1306_i2c_driver.h>
SSD1306_I2C_Driver i2c_driver(0x3D, OLED_RESET); // initialize with the I2C addr 0x3D (for the 128x64)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &i2c_driver);
...
display.begin(SSD1306_SWITCHCAPVCC)

```

See examples and header files for more details


<!-- START COMPATIBILITY TABLE -->

## Compatibility

MCU         |Tested Works|Doesn't Work|Not Tested|Notes
------------|:----------:|:----------:|:--------:|-----
Atmega328   |      X     |            |          |
Atmega32u4  |      X     |            |          |
Atmega2560  |      X     |            |          |
ESP8266     |      X     |            |          | Change OLED_RESET to different pin if using default I2C pins D4/D5.
ESP32       |      X     |            |          |
ATSAM3X8E   |      X     |            |          |
ATSAM21D    |      X     |            |          |
Intel Curie |      X     |            |          |
WICED       |      X     |            |          | No hardware SPI - bitbang only
ATtiny85    |            |      X     |          |
STM32F1     |      X     |            |          | Tested with STM32GENERIC and stm32duino


  * ATmega328 : Arduino UNO, Adafruit Pro Trinket, Adafruit Metro 328, Adafruit Metro Mini
  * ATmega32u4 : Arduino Leonardo, Arduino Micro, Arduino Yun, Teensy 2.0, Adafruit Flora, Bluefruit Micro
  * ATmega2560 : Arduino Mega
  * ESP8266 : Adafruit Huzzah
  * ATSAM3X8E : Arduino Due
  * ATSAM21D : Arduino Zero, M0 Pro, Adafruit Metro Express, Feather M0
  * ATtiny85 : Adafruit Gemma, Arduino Gemma, Adafruit Trinket
  * STM32F1: Blue Pill board (STM32F103C8)

<!-- END COMPATIBILITY TABLE -->
