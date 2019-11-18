# Adafruit_SSD1306 [![Build Status](https://travis-ci.org/adafruit/Adafruit_SSD1306.svg?branch=master)](https://travis-ci.org/adafruit/Adafruit_SSD1306)

## Description

SSD1306 OLED driver library for monochrome 128x64 and 128x32 displays. These displays use I2C or SPI to communicate, 2 to 5 pins are required to interface. SSD1306 can communicate in two ways: I2C or SPI.

Pick one up today in the [adafruit shop](http://www.adafruit.com/category/63_98)!

## Installation

### First Method

![image](https://user-images.githubusercontent.com/36513474/68978714-5b79bc80-081d-11ea-98a4-91d91d24cecd.png)

1. In the Arduino IDE, navigate to Sketch > Include Library > Manage Libraries
1. Then the Library Manager will open and you will find a list of libraries that are already installed or ready for installation.
1. Then search for Adafruit SSD1306 using the search bar.
1. Click on the text area and then select the specific version and install it.

### Second Method

1. Navigate to the [Releases page](https://github.com/adafruit/Adafruit_SSD1306/releases).
1. Download the latest release.
1. Extract the zip file
1. In the Arduino IDE, navigate to Sketch > Include Library > Add .ZIP Library

## Requirements

This library depends on  Adafruit_GFX  being present on your system. Please make sure you have installed the latest version before using this library. It provides graphics primitives such as lines, circles, text, etc.  This also can be found in the Arduino Library Manager, or you can get the source from [here](https://github.com/adafruit/Adafruit-GFX-Library)

## Features

- ### Simple to use

  The code is simple to adapt to any other microcontroller.

- ### Fast

  It uses 1K of RAM since it needs to buffer the entire display but it’s very fast

- ### Give back

  The library is free; you don’t have to pay for anything. Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

- ### BSD License

  BSD license, all text must be included in any redistribution. Click [license.txt](https://github.com/adafruit/Adafruit_SSD1306/blob/master/license.txt) for more information.

## Functions

- SPIwrite()
- ssd1306_command1()
- ssd1306_commandList()
- ssd1306_command()
- begin()
- drawPixel()
- clearDisplay()
- drawFastHLine()
- drawFastHLineInternal()
- drawFastVLine()
- drawFastVLineInternal()
- getPixel()
- getBuffer()
- display()
- startscrollright()
- startscrollleft()
- startscrolldiagright()
- startscrolldiagleft()
- stopscroll()
- invertDisplay()
- dim()

## Contributing

If you want to contribute to this project:

- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell others about this library
- Contribute new protocols

Please read [CONTRIBUTING.md](https://github.com/adafruit/Adafruit_SSD1306/blob/master/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Credits

This library is written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. Scrolling code contributed by Michael Gregg. Dynamic buffer allocation based on work by Andrew Canaday.

## Changes

Pull Request:
   (September 2019)
   - new #defines for SSD1306_BLACK, SSD1306_WHITE and SSD1306_INVERSE that match existing #define naming scheme and won't conflict with common color names.
   - old #defines for BLACK, WHITE and INVERSE kept for backwards compat (opt-out with #define NO_ADAFRUIT_SSD1306_COLOR_COMPATIBILITY)

Version 1.2 (November 2018) introduces some significant changes:

  - Display dimensions are now specified in the constructor...you no longer need to edit the .h file for different screens (though old sketches can continue to work that way).
  - SPI transactions are used and SPI bitrate can be specified (both require Arduino 1.6 or later).
  - SPI and Wire (I2C) interfaces other than the defaults are supported.

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

- ATmega328 : Arduino UNO, Adafruit Pro Trinket, Adafruit Metro 328, Adafruit Metro Mini
- ATmega32u4 : Arduino Leonardo, Arduino Micro, Arduino Yun, Teensy 2.0, Adafruit Flora, Bluefruit Micro
- ATmega2560 : Arduino Mega
- ESP8266 : Adafruit Huzzah
- ATSAM3X8E : Arduino Due
- ATSAM21D : Arduino Zero, M0 Pro, Adafruit Metro Express, Feather M0
- ATtiny85 : Adafruit Gemma, Arduino Gemma, Adafruit Trinket

<!-- END COMPATIBILITY TABLE -->
## License

BSD license, all text above, and the splash screen included below, must be included in any redistribution. You can read the Software License Agreement [here](https://github.com/adafruit/Adafruit_SSD1306/blob/master/license.txt).
