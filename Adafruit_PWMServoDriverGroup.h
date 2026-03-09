/*!
 *  @file Adafruit_PWMServoDriverGroup.h
 *
 *  This Library sets up a group of Adafruit_PWMServoDrivers
 *  to act like a single Adafruit_PWMServoDrivers object
 *
 *
 *  Designed specifically to work with the Adafruit 16-channel PWM & Servo
 * driver.
 *
 *  BSD license, all text above must be included in any redistribution
 */
#ifndef _ADAFRUIT_PWMServoDriverGroup_H
#define _ADAFRUIT_PWMServoDriverGroup_H

#include "Adafruit_PWMServoDriver.h"

#define PCA9685_I2C_ADDRESS_1 0x40 /**< Default PCA9685 I2C Slave Address */
#define PCA9685_I2C_ADDRESS_2 0x41 /**< Second I2C Slave Address */
#define PCA9685_I2C_ADDRESS_3 0x42 /**< Third I2C Slave Address */
#define PCA9685_I2C_ADDRESS_4 0x43 /**< Fourth I2C Slave Address */
#define PCA9685_I2C_ADDRESS_5 0x44 /**< Fifth I2C Slave Address */

/*!
 *  @brief  Class that stores state and functions for interacting with multiple
 * PCA9685 PWM chips as if they are a single instance
 */
class Adafruit_PWMServoDriverGroup {
public:
  Adafruit_PWMServoDriverGroup(const uint8_t ndrivers,
                               const uint8_t nServosEach, const uint8_t *addr);
  Adafruit_PWMServoDriverGroup(const uint8_t ndrivers,
                               const uint8_t nServosEach, const uint8_t *addr,
                               TwoWire &i2c);

  Adafruit_PWMServoDriver *getDriver(uint8_t num, uint8_t &localNum);
  bool begin(uint8_t prescale = 0);
  void reset();
  void sleep();
  void wakeup();
  void setExtClk(uint8_t prescale);
  void setPWMFreq(float freq);
  void setOutputMode(bool totempole);
  uint16_t getPWM(uint8_t num, bool off = false);
  uint8_t setPWM(uint8_t num, uint16_t on, uint16_t off);
  void setPin(uint8_t num, uint16_t val, bool invert = false);
  uint8_t readPrescale(void);
  void writeMicroseconds(uint8_t num, uint16_t microseconds);
  void setOscillatorFrequency(uint32_t freq);
  void setOscillatorFrequency(uint8_t id, uint32_t freq);
  uint32_t getOscillatorFrequency(uint8_t id);

  uint8_t getNumDrivers();
  uint8_t getNumServos();
  uint8_t getNumServosEach();

private:
  uint8_t _nDrivers;
  uint8_t _nServosEach;
  Adafruit_PWMServoDriver **_drivers;
};

#endif