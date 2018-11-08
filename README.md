# Adafruit_SSD1306

This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 2 to 5 pins are required to interface.

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. Scrolling code contributed by Michael Gregg. Dynamic buffer allocation based on work by Andrew Canaday.
BSD license, check license.txt for more information. All text above must be included in any redistribution

Preferred installation method is to use the Arduino IDE Library Manager. To download the source from Github instead, click "Clone or download" above, then "Download ZIP." After uncompressing, rename the resulting folder Adafruit_SSD1306. Check that the Adafruit_SSD1306 folder contains Adafruit_SSD1306.cpp and Adafruit_SSD1306.h.

You will also have to install the Adafruit GFX library which provides graphics primitves such as lines, circles, text, etc. This also can be found in the Arduino Library Manager, or you can get the source from https://github.com/adafruit/Adafruit-GFX-Library


<!-- START COMPATIBILITY TABLE -->

## Compatibility

MCU                |Tested Works|Doesn't Work|Not Tested |Notes
-------------------|:----------:|:----------:|:---------:|-----
Atmega328 @ 16MHz  |      X     |            |           |
Atmega328 @ 12MHz  |      X     |            |           |
Atmega32u4 @ 16MHz |      X     |            |           |
Atmega32u4 @ 8MHz  |      X     |            |           |
ESP8266            |      X     |            |           | change OLED_RESET to different pin if using default I2C pins D4/D5.
Atmega2560 @ 16MHz |      X     |            |           |
ATSAM3X8E          |      X     |            |           |
ATSAM21D           |      X     |            |           |
ATtiny85 @ 16MHz   |            |      X     |           |
ATtiny85 @ 8MHz    |            |      X     |           |
WICED              |            |      X     |           |
ESP32              |            |      X     |           |
Intel Curie @ 32MHz|            |            |     X     |
STM32F2            |            |            |     X     |

  * ATmega328 @ 16MHz : Arduino UNO, Adafruit Pro Trinket 5V, Adafruit Metro 328, Adafruit Metro Mini
  * ATmega328 @ 12MHz : Adafruit Pro Trinket 3V
  * ATmega32u4 @ 16MHz : Arduino Leonardo, Arduino Micro, Arduino Yun, Teensy 2.0
  * ATmega32u4 @ 8MHz : Adafruit Flora, Bluefruit Micro
  * ESP8266 : Adafruit Huzzah
  * ATmega2560 @ 16MHz : Arduino Mega
  * ATSAM3X8E : Arduino Due
  * ATSAM21D : Arduino Zero, M0 Pro
  * ATtiny85 @ 16MHz : Adafruit Trinket 5V
  * ATtiny85 @ 8MHz : Adafruit Gemma, Arduino Gemma, Adafruit Trinket 3V

<!-- END COMPATIBILITY TABLE -->

## Changes

Version 1.2 (November 2018) introduces some significant changes:

  * Display dimensions are now specified in the constructor...you no longer need to edit the .h file for different screens (though old sketches can continue to work that way).
  * SPI transactions are used and SPI bitrate can be specified (both require Arduino 1.6 or later).
  * SPI and Wire (I2C) interfaces other than the defaults are supported.
