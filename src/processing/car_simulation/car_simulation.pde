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

/*
  Sets up the pictures and the serial object.
*/
void setup() {
  // new Serial object for reading input from arduino.
	port = new Serial(this, Serial.list()[0], 9600);
	port.bufferUntil(10);
  
  // set up car pictures.
	carBottomBlank = loadImage("car_pics/car2_bottom_blank.png");  
 	carLeft = loadImage("car_pics/car2_left.png");
	carReverseLeft = loadImage("car_pics/car2_left_reverse.png");
	carReverse = loadImage("car_pics/car2_reverse.png");
	carRight = loadImage("car_pics/car2_right.png");
	carReverseRight = loadImage("car_pics/car2_right_reverse.png");
	carTopBlank = loadImage("car_pics/car2_top_blank.png");
	carTopBrake = loadImage("car_pics/car2_top_brake.png");

	size(505, 389);  
  // show the blank car
	image(carTopBlank, 0, 0);
	image(carBottomBlank, 0, CAR_BOTTOM_H);
}

boolean blink = false;  // responisble for the blinking.

/*
  Infinite loop. 
  Displays the car pictures.
*/
void draw() {
	int[] vals = readIn();  // get the values from Serial

	if (vals == null) {  // data recieved was incomplete.
    	println("RETURNED"); 
    	return;
	}
  
  // split the data into it's respective parts.
	int brake = vals[2]; 
	int x_axis = vals[1];
	int y_axis = vals[0];
  
  // if the brake is on, show the top brake picture
	image ((brake == 1 ? carTopBrake : carTopBlank), 0, 0);

	if (blink) {  // responsible for blink effect
		if (y_axis == 1) {  // backing up
			image(carReverse, 0, CAR_BOTTOM_H);
		} else {  // normal position (blank)
			image(carBottomBlank, 0, CAR_BOTTOM_H);
		}
	} else {  // not blinking, therefore show some images
		if (x_axis == 2) {  // turning right
			if (y_axis == 1) {  // if also reversing
				image(carReverseRight, 0, CAR_BOTTOM_H);
			} else {  // just turning right
				image(carRight, 0, CAR_BOTTOM_H);
			}
		} else if (x_axis == 1) {  // turning left
			if (y_axis == 1) {  // if also reversing
				image(carReverseLeft, 0, CAR_BOTTOM_H);
			} else {  // just turning left
				image(carLeft, 0, CAR_BOTTOM_H);
			}
		} else {  // normal position
			if (y_axis == 1) {  // backing up
				image(carReverse, 0, CAR_BOTTOM_H);
			} else {  // normal position, blank car
				image(carBottomBlank, 0, CAR_BOTTOM_H);
			}
		}
	}
  
  // flip the blink flag
	blink = !blink;  // can't update image in a loop, therefore draw() is used as loop
}

/*
  Reads the serial port and parses CSV values.
 @returns {@code null} if values recieved are incomplete,
 or an integer array (of y,x,sw).
 */
int[] readIn() {
	delay(500);  // for the blink

	if (port.available() > 0) {  // check for input
		String read = port.readStringUntil(10);  // still reads '\n', haven't looked into why.
   
		if (read == null) {  // if there was no data.
			println("NULL");
			return null;
		}
    
    // parse the csv data (replace '\n' with nothing...
    // because the readString() method still reads '\n').
		read = read.replace("\n", "").trim();
		String[] parser = read.split(",");

		//println(read);
    
    // not enough data
		if (parser.length < 3) {  // 3 == x, y, brake
			println("ERR INC"); 
			return null;
		}
    
    // try to parse the data into integers
		try {	
			int y_axis = Integer.parseInt(parser[0]);
			print(y_axis + ",");  // print out the data (to see input)

			int x_axis = Integer.parseInt(parser[1]);
			print(x_axis + ",");

			int brake_switch = Integer.parseInt(parser[2]);
			println(brake_switch);
      
      // return the data in an array
			return new int[]{y_axis, x_axis, brake_switch};
		} catch(NumberFormatException nfe) {
			println("ERR FMT");
			return null;
		}
	}

	return null;  // invalid data.
}
