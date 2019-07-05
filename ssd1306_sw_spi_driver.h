#ifndef SSD1306_SW_SPI_DRIVER_H
#define SSD1306_SW_SPI_DRIVER_H

#include "Adafruit_SSD1306.h"

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

/*! 
  @brief Software SPI driver for SSD1306 display
     
  This class implements communication over software implementation of SPI interface. The class
  encapsulate all the work related to hardware interfaces including initialization used pins, 
  and transferring the data
*/
class SSD1306_SW_SPI_Driver : public ISSD1306Driver
{
  // SPI connection use standard MOSI and SCLK pins plus Data/Command and Chip Select pins
  int8_t       mosiPin,     clkPin,      dcPin,      csPin;
#ifdef HAVE_PORTREG
  PortReg     *mosiPort   , *clkPort   , *dcPort   , *csPort;
  PortMask     mosiPinMask,  clkPinMask,  dcPinMask,  csPinMask;
#endif

  // Driver may also reset the display if dedicated line is connected to rstPin
  int8_t rstPin;

  void fastSPIwrite(uint8_t c);

public:
  /*!
    @brief  Constructor for software SPI driver.
    @param  mosi_pin
            MOSI (master out, slave in) pin (using Arduino pin numbering).
            This transfers serial data from microcontroller to display.
    @param  sclk_pin
            SCLK (serial clock) pin (using Arduino pin numbering).
            This clocks each bit from MOSI.
    @param  dc_pin
            Data/command pin (using Arduino pin numbering), selects whether
            display is receiving commands (low) or data (high).
    @param  cs_pin
            Chip-select pin (using Arduino pin numbering) for sharing the
            bus with other devices. Active low.
    @param  rst_pin
            Reset pin if specified (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin). If the pin is not -1, a hard reset will be performed before
            initializing the display. If using multiple SSD1306 displays on the same
            bus, and if they all share the same reset pin, you should only pass reset
            pin number on the first display being initialized, and -1 on all others
            Default if unspecified is -1.
    @return SSD1306_SW_SPI_Driver object.
  */
  SSD1306_SW_SPI_Driver(int8_t mosi_pin, int8_t sclk_pin, int8_t dc_pin, int8_t cs_pin, int8_t rst_pin = -1);

  virtual void begin();
  virtual void startTransaction();
  virtual void sendCommand(uint8_t cmd);
  virtual void sendCommands(const uint8_t *cmds, size_t size);
  virtual void sendData(const uint8_t * data, size_t size);
  virtual void endTransaction();
};

#endif // SSD1306_SPI_DRIVER_H
