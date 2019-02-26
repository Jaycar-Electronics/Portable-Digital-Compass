//https://github.com/sparkfun/SparkFun_MAG3110_Breakout_Board_Arduino_Library/archive/master.zip
#include <SparkFun_MAG3110.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // Version 1.2.9
#include <Wire.h>

#include "./startup_bitmap.h"

#define text_offset 34
#define char_height 8 //character height
#define screen_width 128
#define screen_height 64

#define half_pi			1.5708
#define pi				3.1415

#define s_div 43		//this is the gap required to split the screen into 3 lengthways
						//128 / 3

// create screen object
Adafruit_SSD1306 screen(screen_width, screen_height, &Wire, 0);

const char *directions[] = { //goes counter clockwise
	"North West",
	"West      ",
	"South West",
	"South     ",
	"South East",
	"East      ",
	"North East",
	"North     "
};
// create compass object
//Compass mag;
MAG3110 mag = MAG3110();


void draw_compass(Adafruit_GFX *screen, uint8_t centre_x, uint8_t centre_y, uint8_t radius, double rad);

void setup(){

	//initialise
	Serial.begin(9600); //debugging to USB
	mag.initialize();
	mag.enterCalMode();
	screen.begin(SSD1306_SWITCHCAPVCC, 0x3C);

	screen.setTextSize(1);
	screen.setTextColor(WHITE);
	screen.clearDisplay();

	//draw the startup logo
	screen.drawBitmap(0,0, compass_startup_bmp, 128, 32, WHITE);
	screen.display();
	delay(1000); // hold it for one second
  screen.clearDisplay();
}

double rad = 0;
int x,y,z;
long timer = 0;
bool flash = true;

void loop(){

	if (mag.isCalibrating()){
		//display calibration screen
		screen.setCursor(0,20);
    screen.print(F("Fetching magnetic      fields"));
    screen.setCursor(20,40);
    
    if( millis() - timer > 1000){   //this will trigger every second
      timer = millis();
      
      //toggle flashing text
      if(flash)
		    screen.print(F("Spin on the spot"));
      else
        screen.print(F("                "));
      flash = !flash;
    }
		mag.calibrate();
   
	}

	if (mag.isCalibrated() && mag.dataReady()){
    screen.clearDisplay(); //clear internal buffer
		mag.readMag(&x, &y, &z);
		int heading = (int) mag.readHeading(); //add screen orientation

		//by default heading ranges from -180 to 180. to make it 360, we just add 180
		heading += 180;
		heading %= 360; // wrap it around 360;

		rad = heading * pi / 180;

		rad = rad > 6.283 ? rad - 6.283 : rad;

		//draw compass function
		draw_compass(&screen, 16, 16, 16, rad);

		//set the cursor to positions and draw strings
		screen.setCursor(text_offset,0);
		screen.print(F("Heading"));
    screen.setCursor(105,0);
    screen.print(heading);

		screen.setCursor(text_offset,0 + char_height + 4);
		screen.print(directions[( (heading+23) / 45 ) % 8]); //23 here is a rough "45/2" - divides 360 circle into 8 for directions


		screen.setCursor(0,			screen_height - (2*char_height));	screen.print(F("x:"));
		screen.setCursor(0,			screen_height - char_height);		screen.print(x);

		screen.setCursor(s_div,		screen_height - (2*char_height));	screen.print(F("y:"));
		screen.setCursor(s_div,		screen_height - char_height);		screen.print(y);

		screen.setCursor(2*s_div,	screen_height - (2*char_height));	screen.print(F("z:"));
		screen.setCursor(2*s_div,	screen_height - char_height);		screen.print(z);

	}

	//blit all this to the screen
	screen.display();
	delay(10);
}

void draw_compass(Adafruit_GFX *screen, uint8_t centre_x, uint8_t centre_y, uint8_t radius, double rad){

	// screen is a pointer to the adafruit GFX object, which is what our screen (SDD1306) derives from,
	// pointers mean we use '->' notation

	screen->drawCircle(centre_x, centre_y, radius, WHITE);

	uint8_t half_radius = radius / 2;

	int ax = sin(rad)  * radius;             int ay = cos(rad)  * radius;
	int bx = sin(rad+half_pi) * half_radius; int by = cos(rad+half_pi) * half_radius;
	int cx = sin(rad-half_pi) * half_radius; int cy = cos(rad-half_pi) * half_radius;


	//draw all x,y points by offsetting their values to the centre of the circle
	//north facing triangle is filled in
	screen->fillTriangle(
		centre_x + ax, centre_y + ay,
		centre_x + bx, centre_y + by,
		centre_x + cx, centre_y + cy, WHITE);

	screen->drawTriangle(
		centre_x - ax, centre_y - ay,	//reverse the outlined
		centre_x + bx, centre_y + by,
		centre_x + cx, centre_y + cy, WHITE);

}
