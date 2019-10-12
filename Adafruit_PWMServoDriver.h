/*!
 *  @file Adafruit_PWMServoDriver.h
 *
 *  This is a library for our Adafruit 16-channel PWM & Servo driver.
 *
 *  Designed specifically to work with the Adafruit 16-channel PWM & Servo driver. 
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/815
 *
 *  These driver use I2C to communicate, 2 pins are required to interface.
 *  For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  BSD license, all text above must be included in any redistribution
 */
#ifndef _ADAFRUIT_PWMServoDriver_H
#define _ADAFRUIT_PWMServoDriver_H

#include <Arduino.h>
#include <Wire.h>

// REGISTER ADDRESSES
#define PCA9685_MODE1        0x00 /**< Mode Register 1 */
#define PCA9685_MODE2        0x01 /**< Mode Register 2 */
#define PCA9685_SUBADR1      0x02 /**< i2c bus address 1 */
#define PCA9685_SUBADR2      0x03 /**< i2c bus address 2 */
#define PCA9685_SUBADR3      0x04 /**< i2c bus address 3 */
#define PCA9685_ALLCALLADR   0x05
#define PCA9685_LED0_ON_L    0x06 /**< LED0 output and brightness control byte 0 */
#define PCA9685_LED0_ON_H    0x07 /**< LED0 output and brightness control byte 1 */
#define PCA9685_LED0_OFF_L   0x08 /**< LED0 output and brightness control byte 2 */
#define PCA9685_LED0_OFF_H   0x09 /**< LED0 output and brightness control byte 3 */
// etc all 16:  LED15_OFF_H 0x45
#define PCA9685_ALLLED_ON_L  0xFA /**< load all the LEDn_ON registers, byte 0 */
#define PCA9685_ALLLED_ON_H  0xFB /**< load all the LEDn_ON registers, byte 1 */
#define PCA9685_ALLLED_OFF_L 0xFC /**< load all the LEDn_OFF registers, byte 0 */
#define PCA9685_ALLLED_OFF_H 0xFD /**< load all the LEDn_OFF registers, byte 1 */
#define PCA9685_PRESCALE     0xFE /**< Prescaler for PWM output frequency */
#define PCA9685_TESTMODE     0xFF

// MODE1 bits
#define MODE1_ALLCAL         0x01
#define MODE1_SUB3           0x02
#define MODE1_SUB2           0x04
#define MODE1_SUB1           0x08
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

// Default PCA9685 I2C Slave Address if A0-A5 are not set
#define PCA9685_I2C_ADDRESS  0x40

#define FREQUENCY_OSCILLATOR 25000000
#define FREQUENCY_CALIBRATED 26075000 // measured 104.3% compared to official 25 MHz
#define FREQUENCY_LEGACY     27777778 // Effect of freq /= 0,9

#define PCA9685_PRESCALE_MIN 3
#define PCA9685_PRESCALE_MAX 255

/*! 
 *  @brief  Class that stores state and functions for interacting with PCA9685 PWM chip
 */
class Adafruit_PWMServoDriver {
 public:
  Adafruit_PWMServoDriver(uint8_t addr = PCA9685_I2C_ADDRESS, TwoWire *I2C = &Wire);
  void begin(uint8_t prescale = 0);
  void reset();
  void sleep();
  void wakeup();
  void setExtClk(uint8_t prescale);
  void setPWMFreq(float freq);
  void setOutputMode(bool totempole);
  uint8_t getPWM(uint8_t num);
  void setPWM(uint8_t num, uint16_t on, uint16_t off);
  void setPin(uint8_t num, uint16_t val, bool invert=false);

 private:
  uint8_t _i2caddr;
  
  TwoWire *_i2c;

  uint8_t read8(uint8_t addr);
  void write8(uint8_t addr, uint8_t d);
};

#endif
