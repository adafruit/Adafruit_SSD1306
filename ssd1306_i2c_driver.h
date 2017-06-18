#ifndef SSD1306_I2C_DRIVER_H
#define SSD1306_I2C_DRIVER_H

#include "Adafruit_SSD1306.h"

#define SSD1306_I2C_ADDRESS   0x3C  // 011110+SA0+RW - 0x3C or 0x3D
// Address for 128x32 is 0x3C
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)

class SSD1306_I2C_Driver : public ISSD1306Driver
{
	// address of the display on I2C bus
	int8_t _i2caddr;

public:
	SSD1306_I2C_Driver(int8_t i2caddr = SSD1306_I2C_ADDRESS);

	virtual void begin();
	virtual void sendCommand(uint8_t cmd);
	virtual void sendData(uint8_t * data, size_t size);
};

#endif // SSD1306_I2C_DRIVER_H
