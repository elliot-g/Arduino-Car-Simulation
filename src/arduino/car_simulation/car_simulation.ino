uint8_t switch_blink = 1;

// joystick pins
const uint8_t x_joy_pin = A0;
const uint8_t y_joy_pin = A1;
const uint8_t brake_switch = 13;

// distance sensor pins
const uint8_t trig_pin = 12;
const uint8_t echo_pin = 11;

const uint8_t motor_pin = 3;
const uint8_t pot_pin = A2;

// constants for determining joystick position
const int left_lim = 200;
const int right_lim = 550;

// led pins
const uint8_t led_r = 4;
const uint8_t led_l = 8;
const uint8_t led_backup = 7;
const uint8_t led_brake = 2;

/*
	Setup loop runs once.
	Readies some components.
*/
void setup() {
	pinMode(trig_pin, OUTPUT);
	pinMode(led_r, OUTPUT);
	pinMode(led_backup, OUTPUT);
	pinMode(led_l, OUTPUT);
	pinMode(led_brake, OUTPUT);

	pinMode(motor_pin, OUTPUT);

	pinMode(echo_pin, INPUT);
	pinMode(brake_switch, INPUT);
	Serial.begin(9600);
	Serial.println(HIGH);
	// attachInterrupt(digitalPinToInterrupt(brake_switch), brake, CHANGE);
	delay(2000); // uneeded.
}

/*
	Loop runs infinitely.
	Reads and sends values over Serial.
*/
void loop() {
	int x_axis = analogRead(x_joy_pin);
	int y_axis = analogRead(y_joy_pin);
	uint8_t brake = digitalRead(brake_switch);

	analogWrite(motor_pin, map(analogRead(pot_pin), 0, 1023, 0, 255));

	if (y_axis > right_lim) {
		// up
		unsigned long distance = get_distance();

		if (distance < 15.0) {
			// too close, begin braking
			brake = 1;
			Serial.print(0);
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
		digitalWrite(led_r, switch_blink);
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
		digitalWrite(led_brake, HIGH);
	} else {
		Serial.print(0);
		digitalWrite(led_brake, LOW);
	}

	Serial.print("\n");

	switch_blink = (switch_blink == 1 ? 0 : 1);
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
