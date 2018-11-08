/**************************************************************************
 This is a library for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 These displays use I2C or SPI to communicate, 2 to 5 pins are required to
 interface.

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing products
 from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries, with contributions
 from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be included in any
 redistribution.
 **************************************************************************/

#ifndef _Adafruit_SSD1306_H_
#define _Adafruit_SSD1306_H_

// ONE of the following three lines must be #defined:
//#define SSD1306_128_64
#define SSD1306_128_32
//#define SSD1306_96_16
// This establishes the screen dimensions in old Adafruit_SSD1306 sketches
// (NEW CODE SHOULD IGNORE THIS, USE THE CONSTRUCTORS THAT ACCEPT WIDTH
// AND HEIGHT ARGUMENTS).

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

#if defined(__AVR__)
  typedef volatile uint8_t  PortReg;
  typedef uint8_t           PortMask;
  #define HAVE_PORTREG
#elif defined(__SAM3X8E__)
  typedef volatile RwReg    PortReg;
  typedef uint32_t          PortMask;
  #define HAVE_PORTREG
#elif defined(__arm__) || defined(ARDUINO_FEATHER52)
  typedef volatile uint32_t PortReg;
  typedef uint32_t          PortMask;
  #define HAVE_PORTREG
#endif

#define BLACK                                           0
#define WHITE                                           1
#define INVERSE                                         2

#define SSD1306_MEMORYMODE                           0x20
#define SSD1306_COLUMNADDR                           0x21
#define SSD1306_PAGEADDR                             0x22
#define SSD1306_SETCONTRAST                          0x81
#define SSD1306_CHARGEPUMP                           0x8D
#define SSD1306_SEGREMAP                             0xA0
#define SSD1306_DISPLAYALLON_RESUME                  0xA4
#define SSD1306_DISPLAYALLON                         0xA5
#define SSD1306_NORMALDISPLAY                        0xA6
#define SSD1306_INVERTDISPLAY                        0xA7
#define SSD1306_SETMULTIPLEX                         0xA8
#define SSD1306_DISPLAYOFF                           0xAE
#define SSD1306_DISPLAYON                            0xAF
#define SSD1306_COMSCANINC                           0xC0
#define SSD1306_COMSCANDEC                           0xC8
#define SSD1306_SETDISPLAYOFFSET                     0xD3
#define SSD1306_SETDISPLAYCLOCKDIV                   0xD5
#define SSD1306_SETPRECHARGE                         0xD9
#define SSD1306_SETCOMPINS                           0xDA
#define SSD1306_SETVCOMDETECT                        0xDB

#define SSD1306_SETLOWCOLUMN                         0x00
#define SSD1306_SETHIGHCOLUMN                        0x10
#define SSD1306_SETSTARTLINE                         0x40

#define SSD1306_EXTERNALVCC                          0x01
#define SSD1306_SWITCHCAPVCC                         0x02

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define SSD1306_DEACTIVATE_SCROLL                    0x2E
#define SSD1306_ACTIVATE_SCROLL                      0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3

// Deprecated size stuff for backwards compatibility with old sketches
#if defined SSD1306_128_64
 #define SSD1306_LCDWIDTH  128
 #define SSD1306_LCDHEIGHT  64
#endif
#if defined SSD1306_128_32
 #define SSD1306_LCDWIDTH  128
 #define SSD1306_LCDHEIGHT  32
#endif
#if defined SSD1306_96_16
 #define SSD1306_LCDWIDTH   96
 #define SSD1306_LCDHEIGHT  16
#endif

class Adafruit_SSD1306 : public Adafruit_GFX {
 public:
  // NEW CONSTRUCTORS -- recommended for new projects
  Adafruit_SSD1306(uint8_t w, uint8_t h,
    int8_t MOSI, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS);
  Adafruit_SSD1306(uint8_t w, uint8_t h, SPIClass *spi,
    int8_t DC, int8_t RST, int8_t CS, uint32_t bitrate=8000000UL);
  Adafruit_SSD1306(uint8_t w, uint8_t h, TwoWire *twi=&Wire, int8_t RST=-1,
    uint32_t res=100000L);

  // DEPRECATED CONSTRUCTORS - for back compatibility, avoid in new projects
  Adafruit_SSD1306(int8_t MOSI, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS);
  Adafruit_SSD1306(int8_t DC, int8_t RST, int8_t CS);
  Adafruit_SSD1306(int8_t RST = -1);

  ~Adafruit_SSD1306(void);

  bool         begin(uint8_t switchvcc=SSD1306_SWITCHCAPVCC,
                 uint8_t i2caddr=0, bool reset=true);
  void         display(void);
  void         clearDisplay(void);
  void         invertDisplay(bool i);
  void         dim(boolean dim);
  void         drawPixel(int16_t x, int16_t y, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void         startscrollright(uint8_t start, uint8_t stop);
  void         startscrollleft(uint8_t start, uint8_t stop);
  void         startscrolldiagright(uint8_t start, uint8_t stop);
  void         startscrolldiagleft(uint8_t start, uint8_t stop);
  void         stopscroll(void);
  void         ssd1306_command(uint8_t c);
  void         ssd1306_commandList(const uint8_t *c, uint8_t n);

 private:
  inline void  SPIwrite(uint8_t d) __attribute__((always_inline));
  void         drawFastHLineInternal(int16_t x, int16_t y, int16_t w,
                 uint16_t color);
  void         drawFastVLineInternal(int16_t x, int16_t y, int16_t h,
                 uint16_t color);

  uint8_t     *buffer;
  SPIClass    *spi;
  TwoWire     *wire;
  int8_t       i2caddr, vccstate, page_end;
  int8_t       mosiPin    ,  clkPin    ,  dcPin    ,  csPin, rstPin;
#ifdef HAVE_PORTREG
  PortReg     *mosiPort   , *clkPort   , *dcPort   , *csPort;
  PortMask     mosiPinMask,  clkPinMask,  dcPinMask,  csPinMask;
#endif
#ifdef SPI_HAS_TRANSACTION
  SPISettings  spiSettings;
#endif
  uint32_t     restoreClk; // Wire speed following SSD1306 transfers
};

#endif // _Adafruit_SSD1306_H_
