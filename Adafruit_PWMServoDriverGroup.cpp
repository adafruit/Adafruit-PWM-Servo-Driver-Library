#include "Adafruit_PWMServoDriverGroup.h"

/*!
 *  @brief  Instantiates new PCA9685 PWM driver chips with user defined
 *  I2C addresses on a TwoWire interface
 *  @param  nDrivers Number of PWM driver chips to instantiate
 *  @param  nServosEach Number of servos to allocate on each driver
 *  @param  addr Array of 7-bit I2C addresses to locate the chips, default is
 *               0x40 through 0x44
 */
Adafruit_PWMServoDriverGroup::Adafruit_PWMServoDriverGroup(
    const uint8_t nDrivers, const uint8_t nServosEach, const uint8_t *addr) {
  _nDrivers = nDrivers;
  _nServosEach = nServosEach;

  _drivers = (Adafruit_PWMServoDriver **)malloc(
      nDrivers * sizeof(Adafruit_PWMServoDriver *));

  for (uint8_t i = 0; i < _nDrivers; i++) {
    _drivers[i] = new Adafruit_PWMServoDriver(addr[i], Wire);
  }
}

/*!
 *  @brief  Instantiates new PCA9685 PWM driver chips with I2C addresses on a
 * TwoWire interface
 *  @param  nDrivers Number of PWM driver chips to instantiate
 *  @param  nServosEach Number of servos to allocate on each driver
 *  @param  addr Array of 7-bit I2C addresses to locate the chips, default is
 *               0x40 thorugh 0x44
 *  @param  i2c  A reference to a 'TwoWire' object that we'll use to communicate
 */
Adafruit_PWMServoDriverGroup::Adafruit_PWMServoDriverGroup(
    const uint8_t nDrivers, const uint8_t nServosEach, const uint8_t *addr,
    TwoWire &i2c) {
  _nDrivers = nDrivers;
  _nServosEach = nServosEach;

  _drivers = (Adafruit_PWMServoDriver **)malloc(
      nDrivers * sizeof(Adafruit_PWMServoDriver *));

  for (uint8_t i = 0; i < _nDrivers; i++) {
    _drivers[i] = new Adafruit_PWMServoDriver(addr[i], i2c);
  }
}

/*!
 *  @brief Gets the number of PCA9685 PWM driver chips connected to this class
    @returns Number of PCA9685 PWM driver chips
 */
uint8_t Adafruit_PWMServoDriverGroup::getNumDrivers() { return _nDrivers; }

/*!
 *  @brief Gets the number of servos associated with each PCA9685 PWM chip
    @returns Number of servos connected to each PCA9685 PWM driver chip
 */
uint8_t Adafruit_PWMServoDriverGroup::getNumServosEach() {
  return _nServosEach;
}

/*!
 *  @brief Gets the total number of servos associated with this
    @returns Total number of servos
 */
uint8_t Adafruit_PWMServoDriverGroup::getNumServos() {
  return _nDrivers * _nServosEach;
}

/*!
 *  @brief  Gets the Adafruit_PWMServoDriver associated with a given servo and
 *  the local servo number on that device
 *  @param  num Number of servo in master list
 *  @param  localId returns the number of the servo as known to the PCA9685 chip
 *  @return The Adafruit_PWMServoDriver associated with the requested servo
 */
Adafruit_PWMServoDriver *
Adafruit_PWMServoDriverGroup::getDriver(uint8_t num, uint8_t &localId) {
  uint8_t driverId = 0;
  if (num > _nServosEach - 1)
    driverId = num / _nServosEach;
  localId = num - (driverId * _nServosEach);

  return _drivers[driverId];
}

/*!
 *  @brief  Setups the I2C interface and hardware
 *  @param  prescale
 *          Sets External Clock (Optional)
 *  @return true if successful, otherwise false
 */
bool Adafruit_PWMServoDriverGroup::begin(uint8_t prescale) {
  bool status = true;
  for (int i = 0; i < _nDrivers; i++)
    status &= _drivers[i]->begin(prescale);

  return status;
}

/*!
 *  @brief  Sends a reset command to the PCA9685 chips over I2C
 */
void Adafruit_PWMServoDriverGroup::reset() {
  for (int i = 0; i < _nDrivers; i++)
    _drivers[i]->reset();
}

/*!
 *  @brief  Puts boards into sleep mode
 */
void Adafruit_PWMServoDriverGroup::sleep() {
  for (int i = 0; i < _nDrivers; i++)
    _drivers[i]->sleep();
}

/*!
 *  @brief  Wakes boards from sleep
 */
void Adafruit_PWMServoDriverGroup::wakeup() {
  for (int i = 0; i < _nDrivers; i++)
    _drivers[i]->wakeup();
}

/*!
 *  @brief  Sets EXTCLK pin to use the external clock
 *  @param  prescale
 *          Configures the prescale value to be used by the external clock
 */
void Adafruit_PWMServoDriverGroup::setExtClk(uint8_t prescale) {
  for (int i = 0; i < _nDrivers; i++)
    _drivers[i]->setExtClk(prescale);
}

/*!
 *  @brief  Sets the PWM frequency for all chips, up to ~1.6 KHz
 *  @param  freq Floating point frequency that we will attempt to match
 */
void Adafruit_PWMServoDriverGroup::setPWMFreq(float freq) {
  for (int i = 0; i < _nDrivers; i++)
    _drivers[i]->setPWMFreq(freq);
}

/*!
 *  @brief  Sets the output mode of the PCA9685s to either
 *  open drain or push pull / totempole.
 *  Warning: LEDs with integrated zener diodes should
 *  only be driven in open drain mode.
 *  @param  totempole Totempole if true, open drain if false.
 */
void Adafruit_PWMServoDriverGroup::setOutputMode(bool totempole) {
  for (int i = 0; i < _nDrivers; i++)
    _drivers[i]->setOutputMode(totempole);
}

/*!
 *  @brief  Gets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to (nDrivers * nServosEach -
 * 1)
 *  @param  off If true, returns PWM OFF value, otherwise PWM ON
 *  @return requested PWM output value
 */
uint16_t Adafruit_PWMServoDriverGroup::getPWM(uint8_t num, bool off) {
  uint8_t localId = 0;
  auto driver = getDriver(num, localId);
  return driver->getPWM(localId, off);
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to (nDrivers * nServosEach -
 * 1)
 *  @param  on At what point in the 4096-part cycle to turn the PWM output ON
 *  @param  off At what point in the 4096-part cycle to turn the PWM output OFF
 *  @return 0 if successful, otherwise 1
 */
uint8_t Adafruit_PWMServoDriverGroup::setPWM(uint8_t num, uint16_t on,
                                             uint16_t off) {
  uint8_t localId = 0;
  auto driver = getDriver(num, localId);
  return driver->setPWM(localId, on, off);
}

/*!
 *   @brief  Helper to set pin PWM output. Sets pin without having to deal with
 * on/off tick placement and properly handles a zero value as completely off and
 * 4095 as completely on.  Optional invert parameter supports inverting the
 * pulse for sinking to ground.
 *   @param  num One of the PWM output pins, from 0 to (nDrivers * nServosEach -
 * 1)
 *   @param  val The number of ticks out of 4096 to be active, should be a value
 * from 0 to 4095 inclusive.
 *   @param  invert If true, inverts the output, defaults to 'false'
 */
void Adafruit_PWMServoDriverGroup::setPin(uint8_t num, uint16_t val,
                                          bool invert) {
  uint8_t localId = 0;
  auto driver = getDriver(num, localId);
  driver->setPin(localId, val, invert);
}

/*!
 *  @brief  Reads set Prescale from PCA9685
 *  @return prescale value
 */
uint8_t Adafruit_PWMServoDriverGroup::readPrescale() {
  return _drivers[0]->readPrescale();
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins based on the input
 * microseconds, output is not precise
 *  @param  num One of the PWM output pins, from 0 to (nDrivers * nServosEach -
 * 1)
 *  @param  microseconds The number of Microseconds to turn the PWM output ON
 */
void Adafruit_PWMServoDriverGroup::writeMicroseconds(uint8_t num,
                                                     uint16_t microseconds) {
  uint8_t localId = 0;
  auto driver = getDriver(num, localId);

  driver->writeMicroseconds(localId, microseconds);
}

/*!
 *  @brief  Getter for the internally tracked oscillator used for freq
 * calculations
 *  @param id The id of the PCA9685 chip to get frequency from
 *  @returns The frequency the PCA9685 thinks it is running at (it cannot
 * introspect)
 */
uint32_t Adafruit_PWMServoDriverGroup::getOscillatorFrequency(uint8_t id) {
  return _drivers[id]->getOscillatorFrequency();
}

/*!
 *  @brief Setter for the internally tracked oscillator used for freq
 * calculations
 *  @param freq The frequency the PCA9685 should use for frequency calculations
 */
void Adafruit_PWMServoDriverGroup::setOscillatorFrequency(uint32_t freq) {
  for (int i = 0; i < _nDrivers; i++)
    _drivers[i]->setOscillatorFrequency(freq);
}

/*!
 *  @brief Setter for the internally tracked oscillator used for freq
 * calculations
 *  @param id The id of the PCA9685 chip to set the frequency on
 *  @param freq The frequency the PCA9685 should use for frequency calculations
 */
void Adafruit_PWMServoDriverGroup::setOscillatorFrequency(uint8_t id,
                                                          uint32_t freq) {
  _drivers[id]->setOscillatorFrequency(freq);
}
