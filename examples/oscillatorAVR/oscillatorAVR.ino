/***************************************************
  This is an example for our Adafruit 16-channel PWM & Servo driver
  to calibrate the frequency of the oscillator clock of the PCA9685.

  Connect the PCA9685 with I2C (Ground, VCC, SCL, SCA) and apply
  voltage on V+. See above not higher than board limits.
  Connect the signal (yellow pin, PWM) of the PCA9685 to your board:
  Default is pin 3, last of first block.
  Default is pin 2 (of your Arduino Uno).

  Formula for prescale to get the targetted frequency (=update_rate) is:
  prescale = round ( osc_clock / 4096 * update_rate) - 1
  rewritten: osc_clock = (prescale + 1) * 4096 * update_rate
  We will measure the real update_rate to assert the real osc_clock.

https://forum.arduino.cc/t/servo-frequency/984914/18

Working frequency: 50-330Hz

hm 330 Hz means a timeperiod of 1/ 330 = 0.00303 seconds
This means each 3 milliseconds a 2 milliseconds pulse is created
hm well this fits into a a 3 milliseconds interval.

The turning speed at 6.8V is 0.12sec / 60 = 0.002 seconds per degree
this means correction of 1 or 2 degrees could be achieved physically withing the 3 millisecond-period of a 330 Hz signal.

a turn of 90 degrees needs 0.12 / 60 * 90 = 0.24 seconds

With a 50 Hz signal wich is a timeperiod of 1/50 = 0.02 seconds
a 0.02 / 0.003 = 6.67 degree is done.
This means only in case of angles smaller than 6.67 degree there is an advantage of a signal-frequency bigger than the standard 50 Hz.

I have doubts if this is nescessary for your robot. Depending on your mechanic high-speed moving can (but must not) cause unwanted oscillations of the robots mechanical components.

  ***************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver PCA9685 = Adafruit_PWMServoDriver(0x40, Wire);
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
// you can also call it with a different address and I2C interface
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Applied frequency in the test: can be changed to get the optimal
// oscillator calibration for your targetted frequency.
#define FREQUENCY 70

#define PIN_SERVO_FEEDBACK 3 // Connect Yellow PWM pin, 3 = last on first block
#define PIN_BOARD_FEEDBACK 2 // 2 => D2

uint8_t prescale = 0;
// loop
#define INTERVAL 1000000  // 1 sec
int32_t lastEvaluation = 0;
uint16_t frozenCounter = 0;

uint32_t totalCounter = 0;
uint32_t totalTime = 0; // in millis
uint32_t realOsciFreq = 0;
uint64_t Sum_realOsciFreq = 0;
uint32_t Average_realOsciFreq = 0;
uint32_t multiplier = 4096;

// interrupt
volatile uint16_t interruptCounter = 0;

void Update_INT_ISR()
{
	interruptCounter++;
}

void setup()
{
	Serial.begin(115200);
	Serial.println(" PCA9685 Oscillator test");

	// set PCA9685
	PCA9685.begin();
	PCA9685.setPWMFreq(FREQUENCY);             // Set some frequency
	PCA9685.setPWM(PIN_SERVO_FEEDBACK,0,2048); // half of time high, half of time low
	prescale = PCA9685.readPrescale();         // read prescale
	Serial.print(" Target frequency: ");
	Serial.println(FREQUENCY);
	Serial.print(" Applied prescale: ");
	Serial.println(prescale);

	// prepare interrupt pin
	pinMode(PIN_BOARD_FEEDBACK, INPUT);
	attachInterrupt(digitalPinToInterrupt(PIN_BOARD_FEEDBACK), Update_INT_ISR, RISING);

	// take a breath and reset to zero
	delay(10);
	interruptCounter = 0;
	lastEvaluation = micros();
}

void loop()
{
	if (micros() - lastEvaluation > INTERVAL)
	{
		// first freeze counters and adjust for new round
		frozenCounter = interruptCounter; // first freeze counter
		interruptCounter -= frozenCounter;
		lastEvaluation += INTERVAL;

		totalCounter += frozenCounter;
		totalTime += 1;

		// only print deviations from targetted frequency
		//if (frozenCounter != FREQUENCY)
		{
			multiplier = 4096;
			realOsciFreq = (prescale + 1) * totalCounter; // first part calcutlation
			// now follows an ugly hack to have maximum precision in 32 bits
			while (((realOsciFreq & 0x80000000) == 0) && (multiplier != 1))
			{
				realOsciFreq <<= 1;
				multiplier >>= 1;
			}
			realOsciFreq /= totalTime;
			if (multiplier) realOsciFreq *= multiplier;
			
			Sum_realOsciFreq = Sum_realOsciFreq + realOsciFreq;
			Average_realOsciFreq = Sum_realOsciFreq/totalTime;
			
			Serial.print(" Timestamp: ");
			Serial.println(totalTime);
			Serial.print(" Freq: ");
			Serial.println(frozenCounter);
			Serial.print(" Counter: ");
			Serial.println(totalCounter);
			Serial.print(" calc.osci.freq: ");
			Serial.println(realOsciFreq);
			Serial.print(" Average_realOsciFreq: ");
			Serial.println(Average_realOsciFreq);
		}
	}
}
