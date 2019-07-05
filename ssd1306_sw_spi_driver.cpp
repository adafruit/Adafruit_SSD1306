#include "ssd1306_sw_spi_driver.h"

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


SSD1306_SW_SPI_Driver::SSD1306_SW_SPI_Driver(int8_t mosi_pin, int8_t sclk_pin, int8_t dc_pin, int8_t cs_pin, int8_t rst_pin)
  : mosiPin(mosi_pin)
  , clkPin(sclk_pin)
  , dcPin(dc_pin)
  , csPin(cs_pin)
  , rstPin(rst_pin)
{
}

void SSD1306_SW_SPI_Driver::begin()
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

  SSD1306_DESELECT

  // MOSI and SCLK outputs  
  pinMode(mosiPin, OUTPUT); 
  pinMode(clkPin , OUTPUT);

#ifdef HAVE_PORTREG
  mosiPort    = (PortReg *)portOutputRegister(digitalPinToPort(mosiPin));
  mosiPinMask = digitalPinToBitMask(mosiPin);
  clkPort     = (PortReg *)portOutputRegister(digitalPinToPort(clkPin));
  clkPinMask  = digitalPinToBitMask(clkPin);
  *clkPort   &= ~clkPinMask; // Clock low
#else
  digitalWrite(clkPin, LOW); // Clock low
#endif

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

inline void SSD1306_SW_SPI_Driver::fastSPIwrite(uint8_t d)
{
  for(uint8_t bit = 0x80; bit; bit >>= 1)
  {
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

void SSD1306_SW_SPI_Driver::startTransaction()
{
  SSD1306_SELECT;
}

void SSD1306_SW_SPI_Driver::sendCommand(uint8_t cmd)
{
  SSD1306_MODE_COMMAND;
  fastSPIwrite(cmd);
}

void SSD1306_SW_SPI_Driver::sendCommands(const uint8_t *cmds, size_t size)
{
  SSD1306_MODE_COMMAND;
  while(size--) fastSPIwrite(pgm_read_byte(cmds++));
}

void SSD1306_SW_SPI_Driver::sendData(const uint8_t * data, size_t size)
{
  SSD1306_MODE_DATA;
  while(size--) fastSPIwrite(*data++);
}

void SSD1306_SW_SPI_Driver::endTransaction()
{
  SSD1306_DESELECT;
}
