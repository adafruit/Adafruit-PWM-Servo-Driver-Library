/*
* Test new library function getPWM() by writing a value to the "ON" and "OFF" registers
* for all 16 channels of PCA9685. 
* Then read the values back to see if thry match what was written.
* Code written for Adafruit ESP32 feather. in VSCode with PlatformIO.
* @section license License
*
* Copyright (c) 2021 Tom Driscoll. 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions: The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS",
* WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Adafruit_PWMServoDriver.h"
#define BAUDRATE 115200 ///<  Baudrate of Serial
#define numTrials 16

// I2C servo interface
Adafruit_PWMServoDriver *pwm;
uint8_t I2C_ADDR = 0x40; // I2C bus address of servo controller
static const uint8_t SERVO_FREQ = 50; // typical servo pulse frequency, Hz
static constexpr float US_PER_BIT = (1e6 / SERVO_FREQ / 4096); // usec per bit @ 12 bit resolution
static const int MIN_SERVO_PULSE_US = 650;                          // minimum pulse width
static const int MAX_SERVO_PULSE_US = 2400;                          // maximum pulse width
static constexpr int SERVO_PULSE_RANGE = (MAX_SERVO_PULSE_US - MIN_SERVO_PULSE_US);           

void setup()
{
  Serial.begin(BAUDRATE);
  Serial.println("Test Adafruit PWM Servo Driver setPWM / getPWM");
  //< instantiate PWM controller
  pwm = new Adafruit_PWMServoDriver(I2C_ADDR);
  pwm->begin();
  pwm->setPWMFreq(SERVO_FREQ);
  uint16_t offValueWrite;
  uint16_t onValueWrite = 0;
  uint16_t offValueRead;
  uint16_t onValueRead;
  uint32_t numMatches = 0;
  for (uint16_t trialNum = 0; trialNum < numTrials; trialNum++)
  {
    
    for (uint8_t ledNum = 0; ledNum < 16; ledNum ++)
    {
      offValueWrite = (ledNum + MIN_SERVO_PULSE_US + SERVO_PULSE_RANGE * (trialNum + 1) / numTrials) / US_PER_BIT;
      //set pwm
      pwm->setPWM(ledNum, onValueWrite, offValueWrite);
      //read pwm
      offValueRead = pwm->getPWM(ledNum, false);
      onValueRead = pwm->getPWM(ledNum, true);
      bool matches = onValueRead == onValueWrite && offValueRead == offValueWrite;
      if (matches)
      {
        numMatches++;
        Serial.print(F("."));
      }
      else
      {
        Serial.print(F("write On/Off: 0/"));
        Serial.print(offValueWrite);
        Serial.print(F(" read On/Off: "));
        Serial.print(onValueRead);
        Serial.print(F("/"));
        Serial.print(offValueRead);
        Serial.println(F("  Error"));
      }
    }
  }
  Serial.println("");
  Serial.print("# matches/#trials: "); Serial.print(numMatches); Serial.print("/");
  Serial.println(16 * numTrials);
}

void loop() {

}
