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

#ifdef __AVR__
 #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
 #include <pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266) && !defined(ESP32) && !defined(__arc__)
 #include <util/delay.h>
#endif

#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include "splash.h"

// SOME DEFINES AND STATIC VARIABLES USED INTERNALLY -----------------------

#define ssd1306_swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

#if ARDUINO >= 100
 #define WIRE_WRITE wire->write
#else
 #define WIRE_WRITE wire->send
#endif

#ifdef __AVR__
  #define WIRECLK 400000L
#else
  #define WIRECLK 1000000L
#endif

#ifdef HAVE_PORTREG
 #define SSD1306_SELECT       *csPort &= ~csPinMask;
 #define SSD1306_DESELECT     *csPort |=  csPinMask;
 #define SSD1306_MODE_COMMAND *dcPort &= ~dcPinMask;
 #define SSD1306_MODE_DATA    *dcPort |=  dcPinMask;
#else
 #define SSD1306_SELECT       digitalWrite(csPin, LOW);
 #define SSD1306_DESELECT     digitalWrite(csPin, HIGH);
 #define SSD1306_MODE_COMMAND digitalWrite(dcPin, LOW);
 #define SSD1306_MODE_DATA    digitalWrite(dcPin, HIGH);
#endif

#ifdef SPI_HAS_TRANSACTION
 #define SPI_TRANSACTION_START        \
  spi->beginTransaction(spiSettings); \
  SSD1306_SELECT                      \
 #define SPI_TRANSACTION_END          \
  SSD1306_DESELECT                    \
  spi->endTransaction();              \
#else
 #define SPI_TRANSACTION_START SSD1306_SELECT
 #define SPI_TRANSACTION_END   SSD1306_DESELECT
#endif

#define TRANSACTION_START              \
  if(wire) wire->setClock(WIRECLK);    \
  else     { SPI_TRANSACTION_START }

#define TRANSACTION_END                \
  if(wire) wire->setClock(restoreClk); \
  else     { SPI_TRANSACTION_END }

// CONSTRUCTORS, DESTRUCTOR ------------------------------------------------

// New constructor for 'soft' SPI
Adafruit_SSD1306::Adafruit_SSD1306(uint8_t w, uint8_t h,
  int8_t MOSI, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS) :
  Adafruit_GFX(w, h), mosiPin(MOSI), clkPin(SCLK), dcPin(DC),
  rstPin(RST), csPin(CS), spi(NULL), wire(NULL), buffer(NULL) {
}

// New constructor for hardware SPI
Adafruit_SSD1306::Adafruit_SSD1306(uint8_t w, uint8_t h,
  SPIClass *spi, int8_t DC, int8_t RST, int8_t CS, uint32_t bitrate) :
  Adafruit_GFX(w, h), spi(spi ? spi : &SPI), wire(NULL),
  mosiPin(-1), clkPin(-1), dcPin(DC), rstPin(RST), csPin(CS), buffer(NULL) {
#ifdef SPI_HAS_TRANSACTION
  spiSettings = SPISettings(bitrate, MSBFIRST, SPI_MODE0);
#endif
}

// New constructor for I2C
Adafruit_SSD1306::Adafruit_SSD1306(
  uint8_t w, uint8_t h, TwoWire *twi, int8_t RST, uint32_t res) :
  Adafruit_GFX(w, h), wire(twi ? twi : &Wire), restoreClk(res), spi(NULL),
  rstPin(RST), mosiPin(-1), clkPin(-1), dcPin(-1), csPin(-1), buffer(NULL) {
}

// Old constructor for 'soft' SPI (deprecated)
Adafruit_SSD1306::Adafruit_SSD1306(
  int8_t MOSI, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS) :
  Adafruit_GFX(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT),
  mosiPin(MOSI), clkPin(SCLK), dcPin(DC), rstPin(RST), csPin(CS),
  spi(NULL), wire(NULL), buffer(NULL) {
}

// Old constructor for hardware SPI (deprecated)
Adafruit_SSD1306::Adafruit_SSD1306(int8_t DC, int8_t RST, int8_t CS) :
  Adafruit_GFX(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT),
  mosiPin(-1), clkPin(-1), dcPin(DC), rstPin(RST), csPin(CS),
  spi(&SPI), wire(NULL), buffer(NULL) {
#ifdef SPI_HAS_TRANSACTION
  spiSettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
#endif
}

// Old constructor for I2C (deprecated)
Adafruit_SSD1306::Adafruit_SSD1306(int8_t RST) :
  Adafruit_GFX(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT),
  rstPin(RST), mosiPin(-1), clkPin(-1), dcPin(-1), csPin(-1),
  spi(NULL), wire(&Wire), buffer(NULL) {
}

Adafruit_SSD1306::~Adafruit_SSD1306(void) {
  if(buffer) {
    free(buffer);
    buffer = NULL;
  }
}

// LOW-LEVEL UTILS ---------------------------------------------------------

// Issue single byte out SPI, either soft or hardware as appropriate.
// SPI transaction/selection must be performed in calling function.
inline void Adafruit_SSD1306::SPIwrite(uint8_t d) {
  if(spi) {
    (void)spi->transfer(d);
  } else {
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
      if(d & bit) *mosiPort |=  mosiPinMask;
      else        *mosiPort &= ~mosiPinMask;
      *clkPort |=  clkPinMask; // Clock high
      *clkPort &= ~clkPinMask; // Clock low
#else
      digitalWrite(mosiPin, d & bit);
      digitalWrite(clkPin , HIGH);
      digitalWrite(clkPin , LOW);
#endif
    }
  }
}

// Issue single command to SSD1306, using I2C or hard/soft SPI as needed.
// Because command calls are often grouped, SPI transaction and selection
// must be started/ended in calling function for efficiency.
void Adafruit_SSD1306::ssd1306_command(uint8_t c) {
  if(wire) { // I2C
    wire->beginTransmission(i2caddr);
    WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C = 0
    WIRE_WRITE(c);
    wire->endTransmission();
  } else { // SPI (hw or soft) -- transaction started in calling function
    SSD1306_MODE_COMMAND
    SPIwrite(c);
  }
}

// Issue list of commands to SSD1306, same rules as above re: transactions.
void Adafruit_SSD1306::ssd1306_commandList(const uint8_t *c, uint8_t n) {
  if(wire) { // I2C
    wire->beginTransmission(i2caddr);
    WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C = 0
    uint8_t bytesOut = 1;
    while(n--) {
      if(bytesOut >= 32) { // Wire uses 32-byte transfer blocks max
        wire->endTransmission();
        wire->beginTransmission(i2caddr);
        WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C = 0
        bytesOut = 1;
      }
      WIRE_WRITE(pgm_read_byte(c++));
      bytesOut++;
    }
    wire->endTransmission();
  } else { // SPI -- transaction started in calling function
    SSD1306_MODE_COMMAND
    while(n--) SPIwrite(pgm_read_byte(c++));
  }
}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

bool Adafruit_SSD1306::begin(uint8_t vcs, uint8_t addr, bool reset) {

  if((!buffer) && !(buffer = (uint8_t *)malloc(WIDTH * ((HEIGHT + 7) / 8))))
    return false;

  clearDisplay();
  if(HEIGHT > 32) {
    drawBitmap((WIDTH - splash1_width) / 2, (HEIGHT - splash1_height) / 2,
      splash1_data, splash1_width, splash1_height, 1);
  } else {
    drawBitmap((WIDTH - splash2_width) / 2, (HEIGHT - splash2_height) / 2,
      splash2_data, splash2_width, splash2_height, 1);
  }

  vccstate = vcs;

  // Setup pin directions
  if(wire) { // Using I2C
    // If I2C address is unspecified, use default
    // (0x3C for 32-pixel-tall displays, 0x3D for all others).
    i2caddr = addr ? addr : ((HEIGHT == 32) ? 0x3C : 0x3D);
    wire->begin();
  } else { // Using one of the SPI modes, either soft or hardware
    pinMode(dcPin, OUTPUT); // Set data/command pin as output
    pinMode(csPin, OUTPUT); // Same for chip select
#ifdef HAVE_PORTREG
    dcPort    = portOutputRegister(digitalPinToPort(dcPin));
    dcPinMask = digitalPinToBitMask(dcPin);
    csPort    = portOutputRegister(digitalPinToPort(csPin));
    csPinMask = digitalPinToBitMask(csPin);
#endif
    SSD1306_DESELECT
    if(spi) { // Hardware SPI
      spi->begin();
    } else {  // Soft SPI
      pinMode(mosiPin, OUTPUT); // MOSI and SCLK outputs
      pinMode(clkPin , OUTPUT);
#ifdef HAVE_PORTREG
      mosiPort    = portOutputRegister(digitalPinToPort(mosiPin));
      mosiPinMask = digitalPinToBitMask(mosiPin);
      clkPort     = portOutputRegister(digitalPinToPort(clkPin));
      clkPinMask  = digitalPinToBitMask(clkPin);
      *clkPort   &= ~clkPinMask; // Clock low
#else
      digitalWrite(clkPin, LOW); // Clock low
#endif
    }
  }

  // Reset SSD1306 if requested and reset pin specified in constructor
  if(reset && (rstPin >= 0)) {
    pinMode(     rstPin, OUTPUT);
    digitalWrite(rstPin, HIGH);
    delay(1);                   // VDD goes high at start, pause for 1 ms
    digitalWrite(rstPin, LOW);  // Bring reset low
    delay(10);                  // Wait 10 ms
    digitalWrite(rstPin, HIGH); // Bring out of reset
  }

  TRANSACTION_START

  // Init sequence
  static const uint8_t PROGMEM init1[] = {
    SSD1306_DISPLAYOFF,                  // 0xAE
    SSD1306_SETDISPLAYCLOCKDIV,          // 0xD5
    0x80,                                // the suggested ratio 0x80
    SSD1306_SETMULTIPLEX };              // 0xA8
  ssd1306_commandList(init1, sizeof(init1));
  ssd1306_command(HEIGHT - 1);

  static const uint8_t PROGMEM init2[] = {
    SSD1306_SETDISPLAYOFFSET,            // 0xD3
    0x0,                                 // no offset
    SSD1306_SETSTARTLINE | 0x0,          // line #0
    SSD1306_CHARGEPUMP };                // 0x8D
  ssd1306_commandList(init2, sizeof(init2));

  ssd1306_command((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

  static const uint8_t PROGMEM init3[] = {
    SSD1306_MEMORYMODE,                  // 0x20
    0x00,                                // 0x0 act like ks0108
    SSD1306_SEGREMAP | 0x1,
    SSD1306_COMSCANDEC };
  ssd1306_commandList(init3, sizeof(init3));

  if((WIDTH == 128) && (HEIGHT == 32)) {
    static const uint8_t PROGMEM init4a[] = {
      SSD1306_SETCOMPINS,                // 0xDA
      0x02,
      SSD1306_SETCONTRAST,               // 0x81
      0x8F };
    ssd1306_commandList(init4a, sizeof(init4a));
  } else if((WIDTH == 128) && (HEIGHT == 64)) {
    static const uint8_t PROGMEM init4b[] = {
      SSD1306_SETCOMPINS,                // 0xDA
      0x12,
      SSD1306_SETCONTRAST };             // 0x81
    ssd1306_commandList(init4b, sizeof(init4b));
    ssd1306_command((vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF);
  } else if((WIDTH == 96) && (HEIGHT == 16)) {
    static const uint8_t PROGMEM init4c[] = {
      SSD1306_SETCOMPINS,                // 0xDA
      0x2,    // ada x12
      SSD1306_SETCONTRAST };             // 0x81
    ssd1306_commandList(init4c, sizeof(init4c));
    ssd1306_command((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0xAF);
  } else {
    // Other screen varieties -- TBD
  }

  ssd1306_command(SSD1306_SETPRECHARGE); // 0xd9
  ssd1306_command((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
  static const uint8_t PROGMEM init5[] = {
    SSD1306_SETVCOMDETECT,               // 0xDB
    0x40,
    SSD1306_DISPLAYALLON_RESUME,         // 0xA4
    SSD1306_NORMALDISPLAY,               // 0xA6
    SSD1306_DEACTIVATE_SCROLL,
    SSD1306_DISPLAYON };                 // Main screen turn on
  ssd1306_commandList(init5, sizeof(init5));

  TRANSACTION_END

  return true; // Success
}

// DRAWING FUNCTIONS -------------------------------------------------------

// Set/clear/invert a single pixel
void Adafruit_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
    // Pixel is in-bounds. Rotate coordinates if needed.
    switch(getRotation()) {
     case 1:
      ssd1306_swap(x, y);
      x = WIDTH - x - 1;
      break;
     case 2:
      x = WIDTH  - x - 1;
      y = HEIGHT - y - 1;
      break;
     case 3:
      ssd1306_swap(x, y);
      y = HEIGHT - y - 1;
      break;
    }
    switch(color) {
     case WHITE:   buffer[x + (y/8)*WIDTH] |=  (1 << (y&7)); break;
     case BLACK:   buffer[x + (y/8)*WIDTH] &= ~(1 << (y&7)); break;
     case INVERSE: buffer[x + (y/8)*WIDTH] ^=  (1 << (y&7)); break;
    }
  }
}

// Clear everything
void Adafruit_SSD1306::clearDisplay(void) {
  memset(buffer, 0, WIDTH * ((HEIGHT + 7) / 8));
}

void Adafruit_SSD1306::drawFastHLine(
  int16_t x, int16_t y, int16_t w, uint16_t color) {
  boolean bSwap = false;
  switch(rotation) {
   case 1:
    // 90 degree rotation, swap x & y for rotation, then invert x
    bSwap = true;
    ssd1306_swap(x, y);
    x = WIDTH - x - 1;
    break;
   case 2:
    // 180 degree rotation, invert x and y, then shift y around for height.
    x  = WIDTH  - x - 1;
    y  = HEIGHT - y - 1;
    x -= (w-1);
    break;
   case 3:
    // 270 degree rotation, swap x & y for rotation,
    // then invert y and adjust y for w (not to become h)
    bSwap = true;
    ssd1306_swap(x, y);
    y  = HEIGHT - y - 1;
    y -= (w-1);
    break;
  }

  if(bSwap) drawFastVLineInternal(x, y, w, color);
  else      drawFastHLineInternal(x, y, w, color);
}

void Adafruit_SSD1306::drawFastHLineInternal(
  int16_t x, int16_t y, int16_t w, uint16_t color) {

  if((y >= 0) && (y < HEIGHT)) { // Y coord in bounds?
    if(x < 0) { // Clip left
      w += x;
      x  = 0;
    }
    if((x + w) > WIDTH) { // Clip right
      w = (WIDTH - x);
    }
    if(w > 0) { // Proceed only if width is positive
      uint8_t *pBuf = &buffer[(y / 8) * WIDTH + x],
               mask = 1 << (y & 7);
      switch(color) {
       case WHITE:               while(w--) { *pBuf++ |= mask; }; break;
       case BLACK: mask = ~mask; while(w--) { *pBuf++ &= mask; }; break;
       case INVERSE:             while(w--) { *pBuf++ ^= mask; }; break;
      }
    }
  }
}

void Adafruit_SSD1306::drawFastVLine(
  int16_t x, int16_t y, int16_t h, uint16_t color) {
  bool bSwap = false;
  switch(rotation) {
   case 1:
    // 90 degree rotation, swap x & y for rotation,
    // then invert x and adjust x for h (now to become w)
    bSwap = true;
    ssd1306_swap(x, y);
    x  = WIDTH - x - 1;
    x -= (h-1);
    break;
   case 2:
    // 180 degree rotation, invert x and y, then shift y around for height.
    x = WIDTH  - x - 1;
    y = HEIGHT - y - 1;
    y -= (h-1);
    break;
   case 3:
    // 270 degree rotation, swap x & y for rotation, then invert y
    bSwap = true;
    ssd1306_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  if(bSwap) drawFastHLineInternal(x, y, h, color);
  else      drawFastVLineInternal(x, y, h, color);
}

// Problem is all here
void Adafruit_SSD1306::drawFastVLineInternal(
  int16_t x, int16_t __y, int16_t __h, uint16_t color) {

  if((x >= 0) && (x < WIDTH)) { // X coord in bounds?
    if(__y < 0) { // Clip top
      __h += __y;
      __y = 0;
    }
    if((__y + __h) > HEIGHT) { // Clip bottom
      __h = (HEIGHT - __y);
    }
    if(__h > 0) { // Proceed only if height is now positive
      // this display doesn't need ints for coordinates,
      // use local byte registers for faster juggling
      uint8_t  y = __y, h = __h;
      uint8_t *pBuf = &buffer[(y / 8) * WIDTH + x];

      // do the first partial byte, if necessary - this requires some masking
      uint8_t mod = (y & 7);
      if(mod) {
        // mask off the high n bits we want to set
        mod = 8 - mod;
        // note - lookup table results in a nearly 10% performance
        // improvement in fill* functions
        // uint8_t mask = ~(0xFF >> mod);
        static const uint8_t PROGMEM premask[8] =
          { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
        uint8_t mask = pgm_read_byte(&premask[mod]);
        // adjust the mask if we're not going to reach the end of this byte
        if(h < mod) mask &= (0XFF >> (mod - h));

        switch(color) {
         case WHITE:   *pBuf |=  mask; break;
         case BLACK:   *pBuf &= ~mask; break;
         case INVERSE: *pBuf ^=  mask; break;
        }
        pBuf += WIDTH;
      }

      if(h >= mod) { // More to go?
        h -= mod;
        // Write solid bytes while we can - effectively 8 rows at a time
        if(h >= 8) {
          if(color == INVERSE) {
            // separate copy of the code so we don't impact performance of
            // black/white write version with an extra comparison per loop
            do {
              *pBuf ^= 0xFF;  // Invert byte
              pBuf  += WIDTH; // Advance pointer 8 rows
              h     -= 8;     // Subtract 8 rows from height
            } while(h >= 8);
          } else {
            // store a local value to work with
            uint8_t val = (color != BLACK) ? 255 : 0;
            do {
              *pBuf = val;    // Set byte
              pBuf += WIDTH;  // Advance pointer 8 rows
              h    -= 8;      // Subtract 8 rows from height
            } while(h >= 8);
          }
        }

        if(h) { // Do the final partial byte, if necessary
          mod = h & 7;
          // this time we want to mask the low bits of the byte,
          // vs the high bits we did above
          // uint8_t mask = (1 << mod) - 1;
          // note - lookup table results in a nearly 10% performance
          // improvement in fill* functions
          static const uint8_t PROGMEM postmask[8] =
            { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
          uint8_t mask = pgm_read_byte(&postmask[mod]);
          switch(color) {
           case WHITE:   *pBuf |=  mask; break;
           case BLACK:   *pBuf &= ~mask; break;
           case INVERSE: *pBuf ^=  mask; break;
          }
        }
      }
    } // endif positive height
  } // endif x in bounds
}

// REFRESH DISPLAY ---------------------------------------------------------

// Push data currently in RAM to SSD1306 display
void Adafruit_SSD1306::display(void) {
  TRANSACTION_START
  static const uint8_t PROGMEM dlist1[] = {
    SSD1306_PAGEADDR,
    0,                        // Page start address
    0xFF,                     // Page end (not really, but works here)
    SSD1306_COLUMNADDR,
    0 };                      // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1));
  ssd1306_command(WIDTH - 1); // Column end address

  uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
  uint8_t *ptr   = buffer;
  if(wire) { // I2C
    uint16_t i;
    wire->beginTransmission(i2caddr);
    WIRE_WRITE((uint8_t)0x40);
    uint8_t bytesOut = 1;
    while(count--) {
      if(bytesOut >= 32) { // Wire uses 32-byte transfer blocks max
        wire->endTransmission();
        wire->beginTransmission(i2caddr);
        WIRE_WRITE((uint8_t)0x40);
        bytesOut = 1;
      }
      WIRE_WRITE(*ptr++);
      bytesOut++;
    }
    wire->endTransmission();
  } else { // SPI
    SSD1306_MODE_DATA
    while(count--) SPIwrite(*ptr++);
  }
  TRANSACTION_END
}

// SCROLLING FUNCTIONS -----------------------------------------------------

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Adafruit_SSD1306::startscrollright(uint8_t start, uint8_t stop){
  TRANSACTION_START
  static const uint8_t PROGMEM scrollList1a[] = {
    SSD1306_RIGHT_HORIZONTAL_SCROLL,
    0X00 };
  ssd1306_commandList(scrollList1a, sizeof(scrollList1a));
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  static const uint8_t PROGMEM scrollList1b[] = {
    0X00,
    0XFF,
    SSD1306_ACTIVATE_SCROLL };
  ssd1306_commandList(scrollList1b, sizeof(scrollList1b));
  TRANSACTION_END
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Adafruit_SSD1306::startscrollleft(uint8_t start, uint8_t stop){
  TRANSACTION_START
  static const uint8_t PROGMEM scrollList2a[] = {
    SSD1306_LEFT_HORIZONTAL_SCROLL,
    0X00 };
  ssd1306_commandList(scrollList2a, sizeof(scrollList2a));
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  static const uint8_t PROGMEM scrollList2b[] = {
    0X00,
    0XFF,
    SSD1306_ACTIVATE_SCROLL };
  ssd1306_commandList(scrollList2b, sizeof(scrollList2b));
  TRANSACTION_END
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Adafruit_SSD1306::startscrolldiagright(uint8_t start, uint8_t stop){
  TRANSACTION_START
  static const uint8_t PROGMEM scrollList3a[] = {
    SSD1306_SET_VERTICAL_SCROLL_AREA,
    0X00 };
  ssd1306_commandList(scrollList3a, sizeof(scrollList3a));
  ssd1306_command(HEIGHT);
  static const uint8_t PROGMEM scrollList3b[] = {
    SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL,
    0X00 };
  ssd1306_commandList(scrollList3b, sizeof(scrollList3b));
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  static const uint8_t PROGMEM scrollList3c[] = {
    0X01,
    SSD1306_ACTIVATE_SCROLL };
  ssd1306_commandList(scrollList3c, sizeof(scrollList3c));
  TRANSACTION_END
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Adafruit_SSD1306::startscrolldiagleft(uint8_t start, uint8_t stop){
  TRANSACTION_START
  static const uint8_t PROGMEM scrollList4a[] = {
    SSD1306_SET_VERTICAL_SCROLL_AREA,
    0X00 };
  ssd1306_commandList(scrollList4a, sizeof(scrollList4a));
  ssd1306_command(HEIGHT);
  static const uint8_t PROGMEM scrollList4b[] = {
    SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL,
    0X00 };
  ssd1306_commandList(scrollList4b, sizeof(scrollList4b));
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  static const uint8_t PROGMEM scrollList4c[] = {
    0X01,
    SSD1306_ACTIVATE_SCROLL };
  ssd1306_commandList(scrollList4c, sizeof(scrollList4c));
  TRANSACTION_END
}

void Adafruit_SSD1306::stopscroll(void){
  TRANSACTION_START
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
  TRANSACTION_END
}

// OTHER HARDWARE SETTINGS -------------------------------------------------

void Adafruit_SSD1306::invertDisplay(bool i) {
  TRANSACTION_START
  ssd1306_command(i ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
  TRANSACTION_END
}

// Dim the display. Pass true to dim display, false for normal brightness
void Adafruit_SSD1306::dim(boolean dim) {
  uint8_t contrast;

  if(dim) {
    contrast = 0; // Dimmed display
  } else {
    contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF;
  }
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  TRANSACTION_START
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
  TRANSACTION_END
}

