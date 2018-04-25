/*************************************************** 
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

// Set to true to print some debug messages, or false to disable them.
//#define ENABLE_DEBUG_OUTPUT


/**************************************************************************/
/*! 
    @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on the Wire interface. On Due we use Wire1 since its the interface on the 'default' I2C pins.
    @param  addr The 7-bit I2C address to locate this chip, default is 0x40
*/
/**************************************************************************/
Adafruit_PWMServoDriver::Adafruit_PWMServoDriver(uint8_t addr) {
  _i2caddr = addr;

#if defined(ARDUINO_SAM_DUE)
  _i2c = &Wire1;
#else
  _i2c = &Wire;
#endif
}

/**************************************************************************/
/*! 
    @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on a TwoWire interface
    @param  i2c  A pointer to a 'Wire' compatible object that we'll use to communicate with
    @param  addr The 7-bit I2C address to locate this chip, default is 0x40
*/
/**************************************************************************/
Adafruit_PWMServoDriver::Adafruit_PWMServoDriver(TwoWire *i2c, uint8_t addr) {
  _i2c = i2c;
  _i2caddr = addr;
}

/**************************************************************************/
/*! 
    @brief  Setups the I2C interface and hardware
	@param  prescale If value is non-zero, configure to use external clock
*/
/**************************************************************************/
void Adafruit_PWMServoDriver::begin(uint8_t prescale) {
  _i2c->begin();
  reset();
  
  if(prescale)
  {
    setExtClk(prescale);
  }
  else
  {
    // set a default frequency
    setPWMFreq(1000);
  }
}


/**************************************************************************/
/*! 
    @brief  Sends a reset command to the PCA9685 chip over I2C
*/
/**************************************************************************/
void Adafruit_PWMServoDriver::reset(void) {
  write8(PCA9685_MODE1, 0x80);
  delay(10);
}

/**************************************************************************/
/*! 
    @brief  Sets the PWM frequency for the entire chip, up to ~1.6 KHz
    @param  freq Floating point frequency that we will attempt to match
*/
/**************************************************************************/
void Adafruit_PWMServoDriver::setPWMFreq(float freq) {
#ifdef ENABLE_DEBUG_OUTPUT
  Serial.print("Attempting to set freq ");
  Serial.println(freq);
#endif

  freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;

#ifdef ENABLE_DEBUG_OUTPUT
  Serial.print("Estimated pre-scale: "); Serial.println(prescaleval);
#endif

  uint8_t prescale = floor(prescaleval + 0.5);
#ifdef ENABLE_DEBUG_OUTPUT
  Serial.print("Final pre-scale: "); Serial.println(prescale);
#endif
  
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  delay(5);
  write8(PCA9685_MODE1, oldmode | 0xa0);  //  This sets the MODE1 register to turn on auto increment.

#ifdef ENABLE_DEBUG_OUTPUT
  Serial.print("Mode now 0x"); Serial.println(read8(PCA9685_MODE1), HEX);
#endif
}

/**************************************************************************/
/*! 
    @brief  Sets EXTCLK pin to use the external clock
	@param  prescale Configures the prescale value to be used by the external clock
*/
/**************************************************************************/
void Adafruit_PWMServoDriver::setExtClk(uint8_t prescale) {
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep, turn off internal oscillator
  
  // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to use the external clock.
  write8(PCA9685_MODE1, (newmode |= 0x40));
  
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  
  delay(5);
  write8(PCA9685_MODE1, (newmode & ~(0x10)) | 0xa0); //clear the SLEEP bit to start
  
  #ifdef ENABLE_DEBUG_OUTPUT
  Serial.print("Mode now 0x"); Serial.println(read8(PCA9685_MODE1), HEX);
#endif
}

/**************************************************************************/
/*! 
    @brief  Sets the PWM output of one of the PCA9685 pins
    @param  num One of the PWM output pins, from 0 to 15
    @param  on At what point in the 4096-part cycle to turn the PWM output ON
    @param  off At what point in the 4096-part cycle to turn the PWM output OFF
*/
/**************************************************************************/
void Adafruit_PWMServoDriver::setPWM(uint8_t num, uint16_t on, uint16_t off) {
#ifdef ENABLE_DEBUG_OUTPUT
  Serial.print("Setting PWM "); Serial.print(num); Serial.print(": "); Serial.print(on); Serial.print("->"); Serial.println(off);
#endif

  _i2c->beginTransmission(_i2caddr);
  _i2c->write(LED0_ON_L+4*num);
  _i2c->write(on);
  _i2c->write(on>>8);
  _i2c->write(off);
  _i2c->write(off>>8);
  _i2c->endTransmission();
}

/**************************************************************************/
/*! 
    @brief  Helper to set pin PWM output. Sets pin without having to deal with on/off tick placement and properly handles a zero value as completely off and 4095 as completely on.  Optional invert parameter supports inverting the pulse for sinking to ground.
    @param  num One of the PWM output pins, from 0 to 15
    @param  val The number of ticks out of 4096 to be active, should be a value from 0 to 4095 inclusive.
    @param  invert If true, inverts the output, defaults to 'false'
*/
/**************************************************************************/
void Adafruit_PWMServoDriver::setPin(uint8_t num, uint16_t val, bool invert)
{
  // Clamp value between 0 and 4095 inclusive.
  val = min(val, (uint16_t)4095);
  if (invert) {
    if (val == 0) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 4095) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, 4095-val);
    }
  }
  else {
    if (val == 4095) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 0) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, val);
    }
  }
}

/*******************************************************************************************/

uint8_t Adafruit_PWMServoDriver::read8(uint8_t addr) {
  _i2c->beginTransmission(_i2caddr);
  _i2c->write(addr);
  _i2c->endTransmission();

  _i2c->requestFrom((uint8_t)_i2caddr, (uint8_t)1);
  return _i2c->read();
}

void Adafruit_PWMServoDriver::write8(uint8_t addr, uint8_t d) {
  _i2c->beginTransmission(_i2caddr);
  _i2c->write(addr);
  _i2c->write(d);
  _i2c->endTransmission();
}
