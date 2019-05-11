#include <SPI.h>

#include "ssd1306_spi_driver.h"

#ifdef HAVE_PORTREG
 #define SSD1306_SELECT       *csPort &= ~csPinMask; ///< Device select
 #define SSD1306_DESELECT     *csPort |=  csPinMask; ///< Device deselect
 #define SSD1306_MODE_COMMAND *dcPort &= ~dcPinMask; ///< Command mode
 #define SSD1306_MODE_DATA    *dcPort |=  dcPinMask; ///< Data mode
#else
 #define SSD1306_SELECT       digitalWrite(csPin, LOW);  ///< Device select
 #define SSD1306_DESELECT     digitalWrite(csPin, HIGH); ///< Device deselect
 #define SSD1306_MODE_COMMAND digitalWrite(dcPin, LOW);  ///< Command mode
 #define SSD1306_MODE_DATA    digitalWrite(dcPin, HIGH); ///< Data mode
#endif


SSD1306_SPI_Driver::SSD1306_SPI_Driver(int8_t dc_pin, int8_t cs_pin, int8_t rst_pin, SPIClass *spi, bool periphBegin, uint32_t bitrate)
  : dcPin(dc_pin)
  , csPin(cs_pin)
  , rstPin(rst_pin)
  , spi(spi)
  , needsInit(periphBegin)
{
#ifdef SPI_HAS_TRANSACTION
  spiSettings = SPISettings(bitrate, MSBFIRST, SPI_MODE0);
#endif
}

void SSD1306_SPI_Driver::begin()
{
  // set pins modes
  pinMode(dcPin, OUTPUT); // Set data/command pin as output
  pinMode(csPin, OUTPUT); // Same for chip select

#ifdef HAVE_PORTREG
  dcPort    = (PortReg *)portOutputRegister(digitalPinToPort(dcPin));
  dcPinMask = digitalPinToBitMask(dcPin);
  csPort    = (PortReg *)portOutputRegister(digitalPinToPort(csPin));
  csPinMask = digitalPinToBitMask(csPin);
#endif

  SSD1306_DESELECT;

  // Set up SPI peripheral
  if(needsInit)
    SPI.begin();

  // Reset SSD1306 if requested and reset pin specified in constructor
  if(rstPin >= 0) {
    pinMode(     rstPin, OUTPUT);
    digitalWrite(rstPin, HIGH);
    delay(1);                   // VDD goes high at start, pause for 1 ms
    digitalWrite(rstPin, LOW);  // Bring reset low
    delay(10);                  // Wait 10 ms
    digitalWrite(rstPin, HIGH); // Bring out of reset
  }
}

void SSD1306_SPI_Driver::startTransaction()
{
  SSD1306_SELECT;

#ifdef SPI_HAS_TRANSACTION
  spi->beginTransaction(spiSettings);
#endif
}

void SSD1306_SPI_Driver::sendCommand(uint8_t cmd)
{
  SSD1306_MODE_COMMAND;
  (void)spi->transfer(cmd);
}

void SSD1306_SPI_Driver::sendCommands(const uint8_t *cmds, size_t size)
{
  SSD1306_MODE_COMMAND;
  while(size--) spi->transfer(pgm_read_byte(cmds++));
}

void SSD1306_SPI_Driver::sendData(const uint8_t * data, size_t size)
{
  SSD1306_MODE_DATA;
  while(size--) spi->transfer(*data++);
}

void SSD1306_SPI_Driver::endTransaction()
{
  SSD1306_DESELECT;

#ifdef SPI_HAS_TRANSACTION
  spi->endTransaction();
#endif
}
