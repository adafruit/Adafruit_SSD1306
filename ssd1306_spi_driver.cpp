#include <SPI.h>

#include "ssd1306_spi_driver.h"

SSD1306_SPI_Driver::SSD1306_SPI_Driver(int8_t DC, int8_t CS)
{
	dc = DC;
	cs = CS;
}

void SSD1306_SPI_Driver::begin()
{
	// set pin directions
	pinMode(dc, OUTPUT);
	pinMode(cs, OUTPUT);

#ifdef HAVE_PORTREG
	csport      = portOutputRegister(digitalPinToPort(cs));
	cspinmask   = digitalPinToBitMask(cs);
	dcport      = portOutputRegister(digitalPinToPort(dc));
	dcpinmask   = digitalPinToBitMask(dc);
#endif

	// Set up SPI peripheral
	SPI.begin();

#ifdef SPI_HAS_TRANSACTION
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#else
	SPI.setClockDivider (4);
#endif
}

void SSD1306_SPI_Driver::sendCommand(uint8_t cmd)
{
#ifdef HAVE_PORTREG
	*csport |= cspinmask;
	*dcport &= ~dcpinmask;
	*csport &= ~cspinmask;
#else
	digitalWrite(cs, HIGH);
	digitalWrite(dc, LOW);
	digitalWrite(cs, LOW);
#endif

	(void)SPI.transfer(cmd);

#ifdef HAVE_PORTREG
	*csport |= cspinmask;
#else
	digitalWrite(cs, HIGH);
#endif
}

void SSD1306_SPI_Driver::sendData(uint8_t * data, size_t size)
{
#ifdef HAVE_PORTREG
	*csport |= cspinmask;
	*dcport |= dcpinmask;
	*csport &= ~cspinmask;
#else
	digitalWrite(cs, HIGH);
	digitalWrite(dc, HIGH);
	digitalWrite(cs, LOW);
#endif

	for (uint16_t i=0; i<size; i++)
		(void)SPI.transfer(data[size]);

#ifdef HAVE_PORTREG
	*csport |= cspinmask;
#else
	digitalWrite(cs, HIGH);
#endif
}
