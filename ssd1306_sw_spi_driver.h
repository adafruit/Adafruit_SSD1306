#ifndef SSD1306_SW_SPI_DRIVER_H
#define SSD1306_SW_SPI_DRIVER_H

#include "Adafruit_SSD1306.h"

/*! 
    @brief I2C driver for SSD1306 display (based on Arduino's TwoWire class)
     
    This class implements communication over I2C using TwoWire interface. The class
    encapsulate all the work related to hardware interfaces including initialization the
    hardware, maintaining communication speed, and transferring the data
*/


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
  SSD1306_SW_SPI_Driver(int8_t mosi_pin, int8_t sclk_pin, int8_t dc_pin, int8_t cs_pin, int8_t rst_pin = -1);

  virtual void begin();
  virtual void startTransaction();
  virtual void sendCommand(uint8_t cmd);
  virtual void sendCommands(const uint8_t *cmds, size_t size);
  virtual void sendData(const uint8_t * data, size_t size);
  virtual void endTransaction();
};

#endif // SSD1306_SPI_DRIVER_H
