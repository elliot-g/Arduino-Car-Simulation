// used for blinking effect
uint8_t switch_blink = 1;

// joystick pins
const uint8_t x_joy_pin = A0;  // x-axis
const uint8_t y_joy_pin = A1;  // y-axis
const uint8_t sw_joy_pin = 13; // button

// brake button
const uint8_t brake_switch = 4;

// distance sensor pins
const uint8_t trig_pin = 12;
const uint8_t echo_pin = 11;

// pin for the DC motor
const uint8_t motor_pin = 10;
// pin for the potentiometer
const uint8_t pot_pin = A2;

// constants for determining joystick position
const int left_lim = 200;
const int right_lim = 600;

// led pins
const uint8_t led_r = 9;
const uint8_t led_l = 8;
const uint8_t led_backup = 2;
const uint8_t led_brake = 5;

// buzzer pin
const uint8_t buzzer_pin = 7;

/*
  Setup loop runs once.
  Readies some components.
*/
void setup() {
	// setup leds
	pinMode(led_r, OUTPUT);
	pinMode(led_backup, OUTPUT);
	pinMode(led_l, OUTPUT);

	// setup the joystick button
	pinMode(sw_joy_pin, INPUT);
	digitalWrite(sw_joy_pin, HIGH);

	pinMode(motor_pin, OUTPUT);

	pinMode(trig_pin, OUTPUT);
	pinMode(echo_pin, INPUT);

	pinMode(brake_switch, INPUT);

	Serial.begin(9600);

	delay(2000); // uneeded, but here for sync.
}

/*
  Loop runs infinitely.
  Reads and sends values over Serial.
*/
void loop() {
	// read the coordinates and direction of the joysticks position
	int x_axis = analogRead(x_joy_pin);
	int y_axis = analogRead(y_joy_pin);

	// read the brake button
	uint8_t brake = digitalRead(brake_switch);

	// set the motor speed to the potentiometer value
	analogWrite(motor_pin, analogRead(pot_pin) / 4);

	// buzzer on when joystick button is pressed
	if (!digitalRead(sw_joy_pin)) {
		// buzz at 1047 freq.
		tone(buzzer_pin, 1047, 500);
	} // there doesn't seem to be a need for noTone();

	// check of the joystick is in the UP position
	if (y_axis > right_lim) {
		// get the value from the ultrasonic sensor
		unsigned long distance = get_distance();

		// if there an object within 15cm of the back of the car
		if (distance < 15.0) {
			// too close, begin braking
			// send brake signal
			brake = 1;
			Serial.print(0);
			// turn the backup lights off
			digitalWrite(led_backup, LOW);
		} else { // move forward
			Serial.print(2);
			digitalWrite(led_backup, LOW);
		}

		// check of the joystick is in the DOWN position
	} else if (y_axis < left_lim) {
		// get the value from the ultrasonic sensor
		unsigned long distance = get_distance();

		// if there an object within 15cm of the back of the car
		if (distance < 15.0) {
			// too close, begin braking
			brake = 1;
			Serial.print(0);
			// turn the backup lights off
			digitalWrite(led_backup, LOW);
		} else { // move backward
			Serial.print(1);

			// turn the buzzer on and off every 500ms (using the blink flag)
			if (switch_blink) {
				tone(buzzer_pin, 1047, 500);
			}
			// turn on the backup lights
			digitalWrite(led_backup, HIGH);
		}
	} else { // car is not moving
		// normal pos
		Serial.print(0);
		digitalWrite(led_backup, LOW);
	}

	// output for processing (csv)
	Serial.print(",");

	// check of the joystick is in the RIGHT position
	if (x_axis > right_lim) {
		Serial.print(2);
		// alternate the right led on and off, while the left led stays off
		analogWrite(led_r, switch_blink ? 255 : switch_blink);
		digitalWrite(led_l, LOW);
	} else if (x_axis <
			   left_lim) { // check of the joystick is in the LEFT position
		Serial.print(1);
		// alternate the left led on and off, while the right led stays off
		digitalWrite(led_r, LOW);
		digitalWrite(led_l, switch_blink);
	} else { // neither -- normal pos
		Serial.print(0);
		// turn the rght and left leds off
		digitalWrite(led_r, LOW);
		digitalWrite(led_l, LOW);
	}

	// output for processing (csv)
	Serial.print(",");

	if (brake) { // if the brake signal is sent
		Serial.print(1);
		// turn on the brake leds
		analogWrite(led_brake, 255);
	} else {
		Serial.print(0);
		// turn off the brake lights
		analogWrite(led_brake, LOW);
	}

	// output for processing (csv)
	Serial.print("\n");

	// ternary operator for switch_blink
	switch_blink = (switch_blink ? 0 : 1);
	delay(500);
}

/*
  (Unused)
*/
void print_val(String what, int val) {
	Serial.print(what);
	Serial.print(": ");
	Serial.println(val);
	Serial.println();
}

/*
  Get the reading of the distance sensor.
  @returns: -1 if distance is too large or too small,
  or the distance read if not.
*/
unsigned long get_distance() {
	// prepare the sensor
	digitalWrite(trig_pin, LOW);
	delayMicroseconds(2);
	digitalWrite(trig_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trig_pin, LOW);

	// send and recieve the signal from the sensor
	unsigned long dist = pulseIn(echo_pin, HIGH) / 58.0;
	// return distance if it is in an acceptable range
	return (dist == 0 || dist > 400) ? -1 : dist;
}
