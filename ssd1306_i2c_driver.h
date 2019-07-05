#ifndef SSD1306_I2C_DRIVER_H
#define SSD1306_I2C_DRIVER_H

#include "Adafruit_SSD1306.h"

// Address for 128x32 is 0x3C
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)
#define SSD1306_I2C_ADDRESS   0x3C  // 011110+SA0+RW - 0x3C or 0x3D

/*! 
  @brief I2C driver for SSD1306 display (based on Arduino's TwoWire class)
     
  This class implements communication over I2C using TwoWire interface. The class
  encapsulate all the work related to hardware interfaces including initialization the
  hardware, maintaining communication speed, and transferring the data
*/

class SSD1306_I2C_Driver : public ISSD1306Driver
{
  // Address of the display on I2C bus
  int8_t i2caddr;
  // A pointer to actual TwoWire interface to use
  TwoWire * wire;
  // Flag indicating the hardware needs initialization
  bool needsInit;
  // Reset pin number, or -1 if no display reset is required  
  int8_t resetPin;

#if ARDUINO >= 157
  uint32_t     wireClk;    // Wire speed for SSD1306 transfers
  uint32_t     restoreClk; // Wire speed following SSD1306 transfers
#endif

public:
  /*!
    @brief  Constructor for I2C driver.
    @param  addr
            The I2C device address. Usually 0x3C for 128x32, and 0x3D for 128x64
    @param  rst_pin
            Reset pin if specified (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin). If the pin is not -1, a hard reset will be performed before
            initializing the display. If using multiple SSD1306 displays on the same
            bus, and if they all share the same reset pin, you should only pass reset
            pin number on the first display being initialized, and -1 on all others
            Default if unspecified is -1.
    @param  twi
            Pointer to an existing TwoWire instance (e.g. &Wire, the
            microcontroller's primary I2C bus).
    @param  periphBegin
            Perform I2C peripheral initialization. TwoWire begin() function
            might be already performed by the calling function if it has unusual
            circumstances (e.g. TWI variants that can accept different SDA/SCL pins,
            or if two SSD1306 instances with different addresses -- only a single
            begin() is needed).
    @param  clkDuring
            Speed (in Hz) for Wire transmissions in SSD1306 library calls.
            Defaults to 400000 (400 KHz), a known 'safe' value for most
            microcontrollers, and meets the SSD1306 datasheet spec.
            Some systems can operate I2C faster (800 KHz for ESP32, 1 MHz
            for many other 32-bit MCUs), and some (perhaps not all)
            SSD1306's can work with this -- so it's optionally be specified
            here and is not a default behavior. (Ignored if using pre-1.5.7
            Arduino software, which operates I2C at a fixed 100 KHz.)
    @param  clkAfter
            Speed (in Hz) for Wire transmissions following SSD1306 library
            calls. Defaults to 100000 (100 KHz), the default Arduino Wire
            speed. This is done rather than leaving it at the 'during' speed
            because other devices on the I2C bus might not be compatible
            with the faster rate. (Ignored if using pre-1.5.7 Arduino
            software, which operates I2C at a fixed 100 KHz.)
    @return SSD1306_I2C_Driver object.
  */
  SSD1306_I2C_Driver(int8_t addr = SSD1306_I2C_ADDRESS, int8_t rst_pin = -1, TwoWire *twi = &Wire, 
                     bool periphBegin = true, uint32_t clkDuring = 400000UL, uint32_t clkAfter = 100000UL);

  virtual void begin();
  virtual void startTransaction();
  virtual void sendCommand(uint8_t cmd);
  virtual void sendCommands(const uint8_t *cmds, size_t size);
  virtual void sendData(const uint8_t * data, size_t size);
  virtual void endTransaction();
};

#endif // SSD1306_I2C_DRIVER_H
