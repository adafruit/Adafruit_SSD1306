#include "ssd1306_sw_spi_driver.h"

SSD1306_SW_SPI_Driver::SSD1306_SW_SPI_Driver(int8_t SID, int8_t SCLK, int8_t DC, int8_t CS)
{
	dc = DC;
	cs = CS;
	sclk = SCLK;
	sid = SID;
}

void SSD1306_SW_SPI_Driver::begin()
{
	// set pin directions
	pinMode(dc, OUTPUT);
	pinMode(cs, OUTPUT);
	pinMode(sid, OUTPUT);
	pinMode(sclk, OUTPUT);

#ifdef HAVE_PORTREG
	csport      = portOutputRegister(digitalPinToPort(cs));
	cspinmask   = digitalPinToBitMask(cs);
	dcport      = portOutputRegister(digitalPinToPort(dc));
	dcpinmask   = digitalPinToBitMask(dc);
	clkport     = portOutputRegister(digitalPinToPort(sclk));
	clkpinmask  = digitalPinToBitMask(sclk);
	mosiport    = portOutputRegister(digitalPinToPort(sid));
	mosipinmask = digitalPinToBitMask(sid);
#endif
}

inline void SSD1306_SW_SPI_Driver::fastSPIwrite(uint8_t d)
{
	for(uint8_t bit = 0x80; bit; bit >>= 1)
	{
#ifdef HAVE_PORTREG
		*clkport &= ~clkpinmask;
		if(d & bit) *mosiport |=  mosipinmask;
		else        *mosiport &= ~mosipinmask;
		*clkport |=  clkpinmask;
#else
		digitalWrite(sclk, LOW);
		if(d & bit) digitalWrite(sid, HIGH);
		else        digitalWrite(sid, LOW);
		digitalWrite(sclk, HIGH);
#endif
	}
}

void SSD1306_SW_SPI_Driver::sendCommand(uint8_t cmd)
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

	fastSPIwrite(cmd);

#ifdef HAVE_PORTREG
	*csport |= cspinmask;
#else
	digitalWrite(cs, HIGH);
#endif
}

void SSD1306_SW_SPI_Driver::sendData(uint8_t * data, size_t size)
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
		fastSPIwrite(data[i]);

#ifdef HAVE_PORTREG
	*csport |= cspinmask;
#else
	digitalWrite(cs, HIGH);
#endif
}
