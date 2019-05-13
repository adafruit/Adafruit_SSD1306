#ifndef SSD1306_SPI_DRIVER_H
#define SSD1306_SPI_DRIVER_H

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

#if defined(ARDUINO_STM32_FEATHER)
  typedef class HardwareSPI SPIClass;
#endif

/*! 
  @brief Hardware SPI driver for SSD1306 display
     
  This class implements communication over the SPI interface. The class encapsulate all the work
  related to hardware interfaces including initialization, configuration, and transferring the data
*/
class SSD1306_SPI_Driver : public ISSD1306Driver
{
  // SPI connection use standard MOSI and SCLK pins plus Data/Command and Chip Select pins
  int8_t      dcPin,      csPin;

#ifdef HAVE_PORTREG
  PortReg     *dcPort,    *csPort;
  PortMask    dcPinMask,  csPinMask;
#endif

  // Driver may also reset the display if dedicated line is connected to rstPin
  int8_t rstPin;

  // The hardware interface to deal with
  SPIClass * spi;

#if defined(SPI_HAS_TRANSACTION)
  SPISettings  spiSettings;
#endif

  // Do a spi->begin() during initialization
  bool needsInit;

public:
  /*!
    @brief  Constructor for SPI driver.
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
    @param  spi
            Pointer to an existing SPIClass instance - the actual hardware interface to
            communicate with.
    @param  periphBegin
            Perform SPI peripheral initialization (in case if caller does not call
            spi->begin() elsewhere)
    @param  bitrate
            SPI clock rate for transfers to this display. Default if
            unspecified is 8000000UL (8 MHz).
    @return SSD1306_SPI_Driver object.
  */
  SSD1306_SPI_Driver(int8_t dc_pin, int8_t cs_pin, int8_t rst_pin = -1, SPIClass *spi = &SPI, bool periphBegin = true, uint32_t bitrate=8000000UL);

  virtual void begin();
  virtual void startTransaction();
  virtual void sendCommand(uint8_t cmd);
  virtual void sendCommands(const uint8_t *cmds, size_t size);
  virtual void sendData(const uint8_t * data, size_t size);
  virtual void endTransaction();
};

#endif // SSD1306_SPI_DRIVER_H
