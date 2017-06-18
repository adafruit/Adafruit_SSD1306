#include <Wire.h>

#if ARDUINO >= 100
#include "Arduino.h"
#define WIRE_WRITE Wire.write
#else
#include "WProgram.h"
#define WIRE_WRITE Wire.send
#endif

#include "ssd1306_i2c_driver.h"

SSD1306_I2C_Driver::SSD1306_I2C_Driver(int8_t i2caddr)
{
	_i2caddr = i2caddr;
}

void SSD1306_I2C_Driver::begin()
{
	// I2C Init
	Wire.begin();

#ifdef __SAM3X8E__
	// Force 400 KHz I2C, rawr! (Uses pins 20, 21 for SDA, SCL)
	TWI1->TWI_CWGR = 0;
	TWI1->TWI_CWGR = ((VARIANT_MCK / (2 * 400000)) - 4) * 0x101;
#endif
}

void SSD1306_I2C_Driver::sendCommand(uint8_t cmd)
{
	uint8_t control = 0x00;   // Co = 0, D/C = 0
	Wire.beginTransmission(_i2caddr);
	Wire.write(control);
	Wire.write(cmd);
	Wire.endTransmission();
}

void SSD1306_I2C_Driver::sendData(uint8_t * data, size_t size)
{
	// save I2C bitrate
#ifdef TWBR
	uint8_t twbrbackup = TWBR;
	TWBR = 12; // upgrade to 400KHz!
#endif

	for (uint16_t i=0; i<size; i++)
	{
		// send a bunch of data in one xmission
		Wire.beginTransmission(_i2caddr);
		WIRE_WRITE(0x40);
		for (uint8_t x=0; x<16; x++)
		{
			WIRE_WRITE(data[i]);
			i++;
		}
		i--;
		Wire.endTransmission();
	}

#ifdef TWBR
	TWBR = twbrbackup;
#endif
}
