/*!
 * @file Adafruit_SSD1306.h
 *
 * This is part of for Adafruit's SSD1306 library for monochrome
 * OLED displays: http://www.adafruit.com/category/63_98
 *
 * These displays use I2C or SPI to communicate. I2C requires 2 pins
 * (SCL+SDA) and optionally a RESET pin. SPI requires 4 pins (MOSI, SCK,
 * select, data/command) and optionally a reset pin. Hardware SPI or
 * 'bitbang' software SPI are both supported.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries, with
 * contributions from the open source community.
 *
 * BSD license, all text above, and the splash screen header file,
 * must be included in any redistribution.
 *
 */

#ifndef _Adafruit_SSD1306_H_
#define _Adafruit_SSD1306_H_

/*=========================================================================
    SSD1306 Displays
    -------------------------------------------------------------------
    SSD1306 display support communication over SPI and I2C interfaces.
    In order to avoid bringing both SPI and I2C code to user firmware at the
    same time (and therefore increasing firmware size and memory consumptuin)
    hardware interaction is extracted to a driver. User can select which
    driver to use according to hardware display connection. Moreover this 
    simplifies porintg of the library code to other platforms - it just 
    requires creation of a new driver, rather than patching the library.

    Classes responsibilities:
    - Adafruit_SSD1306 knows how to draw pixels (over Adafruit_GFX), knows
      which commands shall be sent to the display, but the actual communication
      is delegated to the driver. Adafruit_SSD1306 class does not have any
      hardware specific code
    - Driver knows how to initialize and then transfer data to the hardware

    The library comes with a few drivers:
    - SSD1306_I2C_Driver - for communication via hardware I2C
    - SSD1306_SPI_Driver - for communication via hardware SPI
    - SSD1306_SW_SPI_Driver - SPI interface software emulation (bitbanging)
    
    Usage:
        SSD1306_I2C_Driver i2c_driver(0x3C);
        Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &i2c_driver);
        
        display.begin();
        display.draw...
        display.display();
  =========================================================================*/

#include <Adafruit_GFX.h>

#define BLACK                          0 ///< Draw 'off' pixels
#define WHITE                          1 ///< Draw 'on' pixels
#define INVERSE                        2 ///< Invert pixels

#define SSD1306_MEMORYMODE          0x20 ///< See datasheet
#define SSD1306_COLUMNADDR          0x21 ///< See datasheet
#define SSD1306_PAGEADDR            0x22 ///< See datasheet
#define SSD1306_SETCONTRAST         0x81 ///< See datasheet
#define SSD1306_CHARGEPUMP          0x8D ///< See datasheet
#define SSD1306_SEGREMAP            0xA0 ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON        0xA5 ///< Not currently used
#define SSD1306_NORMALDISPLAY       0xA6 ///< See datasheet
#define SSD1306_INVERTDISPLAY       0xA7 ///< See datasheet
#define SSD1306_SETMULTIPLEX        0xA8 ///< See datasheet
#define SSD1306_DISPLAYOFF          0xAE ///< See datasheet
#define SSD1306_DISPLAYON           0xAF ///< See datasheet
#define SSD1306_COMSCANINC          0xC0 ///< Not currently used
#define SSD1306_COMSCANDEC          0xC8 ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET    0xD3 ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 ///< See datasheet
#define SSD1306_SETPRECHARGE        0xD9 ///< See datasheet
#define SSD1306_SETCOMPINS          0xDA ///< See datasheet
#define SSD1306_SETVCOMDETECT       0xDB ///< See datasheet

#define SSD1306_SETLOWCOLUMN        0x00 ///< Not currently used
#define SSD1306_SETHIGHCOLUMN       0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE        0x40 ///< See datasheet

#define SSD1306_EXTERNALVCC         0x01 ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27 ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2E ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 ///< Set scroll range


/*! 
    @brief  Hardware driver Interface 
     
    The Adafruit_SSD1306 does not work directly with the hardware
    All the communcation requests are forwarded to the driver which
    actually communicate with the hardware. 
*/
class ISSD1306Driver
{
public:
  /*!
    @brief  Initialize display driver and hardware underneath

    Adafruit_SSD1306 calls this method during its begin() method            
  */
  virtual void begin() = 0;

  /*!
    @brief  Prepare hardware for a communication transaction

    Driver implementation may perform some communication preparations here, e.g.
    set up a communication channel, prepare target device for the data transfer.
  */
  virtual void startTransaction() = 0;

  /*!
    @brief  Send a single command to the display

    @param  cmd - A command to send    
  */
  virtual void sendCommand(uint8_t cmd) = 0;

  /*!
    @brief  Send multiple commands to the display

    @param  cmds - Pointer to the commands list to send
    @param  size - number of commands to send
  
    @note   cmds is a pointer in program memory, not RAM. Implementation must 
            read commands using pgm_read_byte
  */
  virtual void sendCommands(const uint8_t *cmds, size_t size) = 0;

  /*!
    @brief  Send data to the display

    @param  data - Pointer to the data buffer to send
    @param  size - number of commands to send
  
    @note   c is a pointer RAM, not program memory.
  */
  virtual void sendData(const uint8_t * data, size_t size) = 0;

  /*!
    @brief Finalize communication transaction

    Driver implementation may perform some communication finalization here, e.g.
    deactivate communication channel, restore channel settings.
  */
  virtual void endTransaction() = 0;
};


/*! 
    @brief  Class that holds a state and contains functions for interacting with
            SSD1306 OLED displays over the supplied driver.

    The Adafruit_SSD1306 class itself is hardware and platform independent. This class
    implements communication logic with SSD1306 displays, while actuall communication with
    the hardware is performed by the driver. 
*/
class Adafruit_SSD1306 : public Adafruit_GFX {
 public:
  Adafruit_SSD1306(uint8_t w, uint8_t h, ISSD1306Driver * drv);
  ~Adafruit_SSD1306(void);

  boolean      begin(uint8_t switchvcc=SSD1306_SWITCHCAPVCC);
  void         display(void);
  void         clearDisplay(void);
  void         invertDisplay(boolean i);
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
  boolean      getPixel(int16_t x, int16_t y);
  uint8_t     *getBuffer(void);

 private:
  void         drawFastHLineInternal(int16_t x, int16_t y, int16_t w,
                 uint16_t color);
  void         drawFastVLineInternal(int16_t x, int16_t y, int16_t h,
                 uint16_t color);
  void         ssd1306_command1(uint8_t c);
  void         ssd1306_commandList(const uint8_t *c, uint8_t n);

  ISSD1306Driver * driver;
  uint8_t     *buffer;
  int8_t       vccstate, page_end;
};

#endif // _Adafruit_SSD1306_H_
