boolean switch_blink = true;
// joystick pins
const uint8_t x_joy_pin = A0;
const uint8_t y_joy_pin = A1;
const uint8_t brake_switch = 2;

// distance sensor pins
const uint8_t trig_pin = 12;
const uint8_t echo_pin = 11;

// constants for determining joystick position
const int left_lim = 200;
const int right_lim = 550;

// led pins
const uint8_t led_r = ;
const uint8_t led_l = ;
const uint8_t led_backup = ;
const uint8_t led_brake = ;

/*
	Setup loop runs once.
	Readies some components.
*/
void setup() {
	pinMode(trig_pin, OUTPUT);
	pinMode(echo_pin, INPUT);
	pinMode(brake_switch, INPUT);
	Serial.begin(9600);
  //attachInterrupt(digitalPinToInterrupt(brake_switch), brake, CHANGE);
	delay(2000);  // uneeded.
}

// unneeded for now
#define BRAKE 4
#define RIGHT 1
#define LEFT 2
#define REV 3
#define NORM 0

/*
	Loop runs infinitely.
	Reads and sends values over Serial.
*/
void loop() {
	int x_axis = analogRead(x_joy_pin);
	int y_axis = analogRead(y_joy_pin);
	uint8_t brake = digitalRead(brake_switch);

  /*
    Serial.print(x_axis);
    Serial.print(",");
    Serial.print(y_axis);
    Serial.print(",");
    Serial.print(brake);
    Serial.print("\n");
  */

	if (y_axis > right_lim) {
		// up
		Serial.print(2);
	} else if (y_axis < left_lim) {
		// down (reversing)
		digitalWrite(reverse_LED, switchBlink);
    	unsigned long distance = get_distance();

		if (distance < 15.0) { 
			// too close, begin braking 
			brake = 1;
			Serial.print(0);
		} else {
			Serial.print(1);
		}
	} else {
		//normal pos
		Serial.print(0);
	}

	Serial.print(",");
	//Serial.print("x = ");

	if (x_axis > right_lim) {
		// right
		Serial.print(2);
		digitalWrite(right_LED, switchBlink);
		digitalWrite(left_LED, LOW);
	} else if (x_axis < left_lim) {
		//left
		Serial.print(1);
		digitalWrite(right_LED, LOW);
		digitalWrite(left_LED, switchBlink);
	} else {
		//neither -- normal pos
		Serial.print(0);
		digitalWrite(right_LED, LOW);
		digitalWrite(left_LED, LOW);
	}

	Serial.print(",");
	//Serial.print("b = ");

	if (brake) {
		Serial.print(1);
		digitalWrite(brake_LED, HIGH);
	} else {
		Serial.print(0);
		digitalWrite(brake_LED, LOW);
	}

	Serial.print("\n");

	//print_val("dist", dist)
	switch_blink = !switch_blink;
	delay(500);  // TODO sort out
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
