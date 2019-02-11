//#include <SparkFun_MAG3110.h>
#include <i2c_MAG3110.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>


#include "./startup_bitmap.h"

#define text_offset 34
#define cw 5 //character width
#define ch 8 //character height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 0

Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MAG3110 mag; //create magnetometer object

String direction_string = "North west";

void setup(){

	Serial.begin(9600); //debugging to USB
	mag.initialize();
	screen.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	screen.setTextSize(1);
	screen.setTextColor(WHITE);
	screen.clearDisplay();
	screen.drawBitmap(0,0, compass_startup_bmp, 128, 32, WHITE);
	screen.display();
	delay(1000); // hold it for one second
	screen.clearDisplay();
}

#define circle_radius 16
#define half_radius 8
#define quater_pi 0.785382
#define half_pi 1.5708

double rad = 0;

void loop(){
	screen.clearDisplay();
	rad += 0.05;
	rad = rad > 6.283 ? rad - 6.283 : rad;

	float xyz[3] = {0,0,0};
	mag.getMeasurement(xyz);
	int heading = 0;
	int x = xyz[0];
	int y = xyz[1];
	int z = xyz[2];
	//heading = mag.readHeading();
	Serial.print(x,DEC);
	Serial.print(y,DEC);
	Serial.println(z,DEC);
	//32 x 32 is what we'd use for the rotating compass
	//draw a circle starting at 16,16   and with a radius of 16 (32 dia.)
	int Cx = 16;
	int Cy = 16;
	screen.drawCircle(Cx,Cy, circle_radius, WHITE);
	//center of the circle is at 16,16

	int ax = sin(rad)  * circle_radius;
	int ay = cos(rad)  * circle_radius;
	int bx = sin(rad+half_pi) * half_radius;
	int by = cos(rad+half_pi) * half_radius;
	int cx = sin(rad-half_pi) * half_radius;
	int cy = cos(rad-half_pi) * half_radius;


	//draw all x,y points by offsetting their values to the centre of the circle
	//north facing triangle is filled in
	screen.fillTriangle(
		Cx + ax, Cy + ay,
		Cx + bx, Cy + by,
		Cx + cx, Cy + cy, WHITE);
	screen.drawTriangle(
		Cx - ax, Cy - ay,	//reverse the outlined
		Cx + bx, Cy + by,
		Cx + cx, Cy + cy, WHITE);

	// -------------------------------------------------- drawing static strings
	// leaving 96 x 32  for text
	screen.setCursor(text_offset,0);
	screen.print(F("Heading"));
	screen.setCursor(text_offset,0 + ch + 4); // under heading, with padding
	screen.println(direction_string.c_str());


	//128 remaining space divided by 3 is ~43
	#define s_div 43

	screen.setCursor(0,			SCREEN_HEIGHT - (2*ch));	screen.print(F("x:")); 
	screen.setCursor(0,			SCREEN_HEIGHT - ch);		screen.print(x); 

	screen.setCursor(s_div,		SCREEN_HEIGHT - (2*ch));	screen.print(F("y:"));
	screen.setCursor(s_div,		SCREEN_HEIGHT - ch);		screen.print(y);

	screen.setCursor(2*s_div,	SCREEN_HEIGHT - (2*ch));	screen.print(F("z:"));
	screen.setCursor(2*s_div,	SCREEN_HEIGHT - ch);		screen.print(z);

	//blit all this to the screen
	screen.display();

	delay(500);
}
