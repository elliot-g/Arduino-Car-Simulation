import processing.serial.*;

Serial port;

PImage carLeft;
PImage carRight;
PImage carBottomBlank;
PImage carReverseLeft;
PImage carReverseRight;
PImage carReverse;
PImage carTopBrake;
PImage carTopBlank; 

final int CAR_BOTTOM_H = 183;

void setup() {
	port = new Serial(this, Serial.list()[0], 9600);
	port.bufferUntil(10);

	carBottomBlank = loadImage("car_pics/car2_bottom_blank.png");  
 	carLeft = loadImage("car_pics/car2_left.png");
	carReverseLeft = loadImage("car_pics/car2_left_reverse.png");
	carReverse = loadImage("car_pics/car2_reverse.png");
	carRight = loadImage("car_pics/car2_right.png");
	carReverseRight = loadImage("car_pics/car2_right_reverse.png");
	carTopBlank = loadImage("car_pics/car2_top_blank.png");
	carTopBrake = loadImage("car_pics/car2_top_brake.png");

	size(505, 389);
	image(carTopBlank, 0, 0);
	image(carBottomBlank, 0, CAR_BOTTOM_H);
}

boolean blink = false;
final int LEFT_LIM = 550;
final int RIGHT_LIM = 200;

void draw() {
  delay(500);
	int[] vals = readIn();

	if (vals == null) {  // data recieved was incomplete.
    	println("RETURNED"); 
    	return;
	}

	int brake = vals[2];
	int x_axis = vals[1];
	int y_axis = vals[0];

	image ((brake == 1 ? carTopBrake : carTopBlank), 0, 0);  // braking or not?

	if (blink) {  // blink effect, TODO a different way?
		if (y_axis == 1) {
			image(carReverse, 0, CAR_BOTTOM_H);
		} else {
			image(carBottomBlank, 0, CAR_BOTTOM_H);
		}
	} else {
		if (x_axis == 2) {
			// right
			if (y_axis == 1) {
				image(carReverseRight, 0, CAR_BOTTOM_H);
			} else {
				image(carRight, 0, CAR_BOTTOM_H);
			}
		} else if (x_axis == 1) {
			//left
			if (y_axis == 1) {
				image(carReverseLeft, 0, CAR_BOTTOM_H);
			} else {
				image(carLeft, 0, CAR_BOTTOM_H);
			}
		} else {
			if (y_axis == 1) {
				// backing up
				image(carReverse, 0, CAR_BOTTOM_H);
			} else {
				image(carBottomBlank, 0, CAR_BOTTOM_H);
			}
		}
	}

	blink = !blink;  // can't update image in a loop, therefore draw() is used as loop
}

/*
  Reads the serial port and parses CSV values.
 @returns {@code null} if values recieved are incomplete,
 or an integer array (of y,x,sw).
 */
int[] readIn() {
	delay(500);  // for the blink, TODO a different way, perhaps a counter?

	if (port.available() > 0) {  // TODO Read chars / bytes or smthn 
		String read = port.readStringUntil(10); //TODO still reads '\n'
   
		if (read == null) {
			println("NULL");
			return null;
		}

		read = read.replace("\n", "").trim();
		String[] parser = read.split(",");

		//println(read);

		if (parser.length < 3) {  // 3 == x, y, brake
			println("ERR INC"); 
			return null;
		}

		try {	
			int y_axis = Integer.parseInt(parser[0]);
			print(y_axis + ",");

			int x_axis = Integer.parseInt(parser[1]);
			print(x_axis + ",");

			int brake_switch = Integer.parseInt(parser[2]);
			println(brake_switch);

			return new int[]{y_axis, x_axis, brake_switch};
		} catch(NumberFormatException nfe) {
			println("ERR FMT");
			return null;
		}
	}

	return null;
}
