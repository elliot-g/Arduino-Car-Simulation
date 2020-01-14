// used for blinking effect
uint8_t switch_blink = 1;

// joystick pins
const uint8_t x_joy_pin = A0;
const uint8_t y_joy_pin = A1;
const uint8_t sw_joy_pin = 13;

// brake button
const uint8_t brake_switch = 4;

// distance sensor pins
const uint8_t trig_pin = 12;
const uint8_t echo_pin = 11;

const uint8_t motor_pin = 10;
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

	pinMode(sw_joy_pin, INPUT);
	digitalWrite(sw_joy_pin, HIGH);

	pinMode(motor_pin, OUTPUT);

	pinMode(trig_pin, OUTPUT);
	pinMode(echo_pin, INPUT);

	pinMode(brake_switch, INPUT);

	Serial.begin(9600);

	delay(2000); // uneeded.
}

// int backup_duration = 500;

/*
  Loop runs infinitely.
  Reads and sends values over Serial.
*/
void loop() {
	int x_axis = analogRead(x_joy_pin);
	int y_axis = analogRead(y_joy_pin);
	uint8_t brake = digitalRead(brake_switch);
	int pot_val = analogRead(pot_pin) / 4;

	analogWrite(motor_pin, analogRead(pot_pin) / 4);

	Serial.print("sw_pin = ");
	Serial.println(digitalRead(sw_joy_pin));
	Serial.print("pot = ");
	Serial.println(pot_val);

	if (!digitalRead(sw_joy_pin)) {
		tone(buzzer_pin, 1047, 500);
	} // there doesn't seem to be a need for noTone();

	if (y_axis > right_lim) {
		// up
		unsigned long distance = get_distance();

		if (distance < 15.0) {
			// too close, begin braking
			brake = 1;
			Serial.print(0);
			digitalWrite(led_backup, LOW);
		} else {
			Serial.print(2);
			digitalWrite(led_backup, LOW);
		}

	} else if (y_axis < left_lim) {
		// down (reversing)
		unsigned long distance = get_distance();

		if (distance < 15.0) {
			// too close, begin braking
			brake = 1;
			Serial.print(0);
			digitalWrite(led_backup, LOW);
		} else {
			Serial.print(1);

			if (switch_blink) {
				tone(buzzer_pin, 1047, 500);
			}
			digitalWrite(led_backup, HIGH);
		}
	} else {
		// normal pos
		Serial.print(0);
		digitalWrite(led_backup, LOW);
	}

	Serial.print(",");

	if (x_axis > right_lim) {
		// right
		Serial.print(2);
		analogWrite(led_r, switch_blink ? 255 : switch_blink);
		digitalWrite(led_l, LOW);
	} else if (x_axis < left_lim) {
		// left
		Serial.print(1);
		digitalWrite(led_r, LOW);
		digitalWrite(led_l, switch_blink);
	} else {
		// neither -- normal pos
		Serial.print(0);
		digitalWrite(led_r, LOW);
		digitalWrite(led_l, LOW);
	}

	Serial.print(",");

	if (brake) {
		Serial.print(1);
		analogWrite(led_brake, 255);
	} else {
		Serial.print(0);
		analogWrite(led_brake, LOW);
	}

	Serial.print("\n");

	switch_blink = (switch_blink ? 0 : 1);
	// switch_blink = !switch_blink;
	delay(500); // TODO sort out
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
   Uses the distance sensor.
   TODO convert to unsigned long.
*/
unsigned long get_distance() {
	digitalWrite(trig_pin, LOW);
	delayMicroseconds(2);
	digitalWrite(trig_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trig_pin, LOW);

	unsigned long dist = pulseIn(echo_pin, HIGH) / 58.0;
	return (dist == 0 || dist > 400) ? -1 : dist;
}
