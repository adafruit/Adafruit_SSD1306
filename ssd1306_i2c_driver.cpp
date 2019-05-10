#include <Wire.h>

#if defined(BUFFER_LENGTH)
 #define WIRE_MAX BUFFER_LENGTH          ///< AVR or similar Wire lib
#elif defined(SERIAL_BUFFER_SIZE)
 #define WIRE_MAX (SERIAL_BUFFER_SIZE-1) ///< Newer Wire uses RingBuffer
#else
 #define WIRE_MAX 32                     ///< Use common Arduino core default
#endif

#if ARDUINO >= 100
 #define WIRE_WRITE wire->write ///< Wire write function in recent Arduino lib
#else
 #define WIRE_WRITE wire->send  ///< Wire write function in older Arduino lib
#endif

#include "ssd1306_i2c_driver.h"

SSD1306_I2C_Driver::SSD1306_I2C_Driver(int8_t addr, int8_t rst_pin, TwoWire *twi, 
  bool periphBegin, uint32_t clkDuring, uint32_t clkAfter)
  : i2caddr(addr)
  , wire(twi)
  , needsInit(periphBegin)
  , resetPin(rst_pin)
#if ARDUINO >= 157
  , wireClk(clkDuring)
  , restoreClk(clkAfter)
#endif
{
}

void SSD1306_I2C_Driver::begin()
{
  // TwoWire begin() function might be already performed by the calling
  // function if it has unusual circumstances (e.g. TWI variants that
  // can accept different SDA/SCL pins, or if two SSD1306 instances
  // with different addresses -- only a single begin() is needed).
  if(needsInit) 
    wire->begin();

  // Reset SSD1306 if requested and reset pin specified in constructor
  if(resetPin >= 0)
  {
    pinMode(     resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    delay(1);                     // VDD goes high at start, pause for 1 ms
    digitalWrite(resetPin, LOW);  // Bring reset low
    delay(10);                    // Wait 10 ms
    digitalWrite(resetPin, HIGH); // Bring out of reset
  }
}

void SSD1306_I2C_Driver::startTransaction()
{
#if (ARDUINO >= 157) && !defined(ARDUINO_STM32_FEATHER)
  wire->setClock(wireClk);
#endif
}

void SSD1306_I2C_Driver::sendCommand(uint8_t cmd)
{
  wire->beginTransmission(i2caddr);
  WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C = 0
  WIRE_WRITE(cmd);
  wire->endTransmission();
}

void SSD1306_I2C_Driver::sendCommands(const uint8_t *c, size_t n)
{
  wire->beginTransmission(i2caddr);
  WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C = 0
  uint8_t bytesOut = 1;
  while(n--) {
    if(bytesOut >= WIRE_MAX) {
      wire->endTransmission();
      wire->beginTransmission(i2caddr);
      WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C = 0
      bytesOut = 1;
    }
    WIRE_WRITE(pgm_read_byte(c++));
    bytesOut++;
  }
  wire->endTransmission();
}

void SSD1306_I2C_Driver::sendData(const uint8_t * data, size_t size)
{
  wire->beginTransmission(i2caddr);
  WIRE_WRITE((uint8_t)0x40);
  uint8_t bytesOut = 1;
  while(size--) {
    if(bytesOut >= WIRE_MAX) {
      wire->endTransmission();
      wire->beginTransmission(i2caddr);
      WIRE_WRITE((uint8_t)0x40);
      bytesOut = 1;
    }
    WIRE_WRITE(*data++);
    bytesOut++;
  }
  wire->endTransmission();
}

void SSD1306_I2C_Driver::endTransaction()
{
#if (ARDUINO >= 157) && !defined(ARDUINO_STM32_FEATHER)
  wire->setClock(restoreClk);
#endif
}
