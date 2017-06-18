#ifndef SSD1306_SW_SPI_DRIVER_H
#define SSD1306_SW_SPI_DRIVER_H

#include "Adafruit_SSD1306.h"

#if defined(__SAM3X8E__)
	typedef volatile RwReg PortReg;
	typedef uint32_t PortMask;
	#define HAVE_PORTREG
#elif defined(__AVR__)
	typedef volatile uint8_t PortReg;
	typedef uint8_t PortMask;
	#define HAVE_PORTREG
#elif defined(ARDUINO_ARCH_SAMD)
	// not supported
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_STM32_FEATHER) || defined(__arc__)
	typedef volatile uint32_t PortReg;
	typedef uint32_t PortMask;
#endif

class SSD1306_SW_SPI_Driver : public ISSD1306Driver
{
	// SPI connection use standard MOSI and SCLK pins plus Data/Command and Chip Select pins
	int8_t sid, sclk, dc, cs;

#ifdef HAVE_PORTREG
	PortReg *mosiport, *clkport, *csport, *dcport;
	PortMask mosipinmask, clkpinmask, cspinmask, dcpinmask;
#endif

	void fastSPIwrite(uint8_t c);

public:
	SSD1306_SW_SPI_Driver(int8_t SID, int8_t SCLK, int8_t DC, int8_t CS);

	virtual void begin();
	virtual void sendCommand(uint8_t cmd);
	virtual void sendData(uint8_t * data, size_t size);
};

#endif // SSD1306_SPI_DRIVER_H
