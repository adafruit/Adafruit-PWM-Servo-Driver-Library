/***************************************************
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _ADAFRUIT_PWMServoDriver_H
#define _ADAFRUIT_PWMServoDriver_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif
#include <Wire.h>

// REGISTER ADDRESSES
#define PCA9685_MODE1        0x00
#define PCA9685_MODE2        0x01
#define PCA9685_SUBADR1      0x02
#define PCA9685_SUBADR2      0x03
#define PCA9685_SUBADR3      0x04
#define PCA9685_ALLCALLADR   0x05
#define PCA9685_LED0_ON_L    0x06
#define PCA9685_LED0_ON_H    0x07
#define PCA9685_LED0_OFF_L   0x08
#define PCA9685_LED0_OFF_H   0x09
// etc all 16:  LED15_OFF_H 0x45
#define PCA9685_ALLLED_ON_L  0xFA // bit 0-7
#define PCA9685_ALLLED_ON_H  0xFB // bit
#define PCA9685_ALLLED_OFF_L 0xFC
#define PCA9685_ALLLED_OFF_H 0xFD
#define PCA9685_PRESCALE     0xFE
#define PCA9685_TESTMODE     0xFF

// MODE1 bits
#define MODE1_ALLCAL         0x01
#define MODE1_SUB3           0x02
#define MODE1_SUB2           0x04
#define MODE1_SUB3           0x08
#define MODE1_SLEEP          0x10
#define MODE1_AI             0x20
#define MODE1_EXTCLK         0x40
#define MODE1_RESTART        0x80
// MODE2 bits
#define MODE2_OUTNE_0        0x01
#define MODE2_OUTNE_1        0x02
#define MODE2_OUTDRV         0x04
#define MODE2_OCH            0x08
#define MODE2_INVRT          0x10

#define OSCILLATOR_CLOCK_FREQUENCY  25000000
#define BETTER_FREQUENCY            26075000 // 4,3% higher

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with PCA9685 PWM chip
*/
/**************************************************************************/
class Adafruit_PWMServoDriver {
 public:
  Adafruit_PWMServoDriver(uint8_t addr = 0x40);
  Adafruit_PWMServoDriver(TwoWire *I2C, uint8_t addr = 0x40);
  void begin(void);
  void reset(void);
  void setPWMFreq(float freq);
  void setPWM(uint8_t num, uint16_t on, uint16_t off);
  void setPin(uint8_t num, uint16_t val, bool invert=false);
  void setClockFreq(uint32_t freq);

 private:
  uint8_t _i2caddr;
  uint32_t _clock_frequency;

  TwoWire *_i2c;

  uint8_t read8(uint8_t addr);
  void write8(uint8_t addr, uint8_t d);
};

#endif
