#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Adafruit_APDS9960.h"

Adafruit_APDS9960 apds;
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);

   Wire1.begin();
  apds.begin(10, APDS9960_AGAIN_4X, 0x39, &Wire1 );
  apds.enableColor(true);

  //enable color sensing mode
  apds.enableColor(true);
   lcd.init();
  
  lcd.backlight();
  lcd.setCursor(0,0);

  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);   
  pinMode(9, OUTPUT); 

}

void setRGB(int _r, int _g, int _b)
{
    analogWrite(11,_r);
    analogWrite(10,_g);
    analogWrite(9,_b);
}

void loop() {
  lcd.clear();
  //create some variables to store the color data in
  uint16_t r, g, b, c;
  
  //wait for color data to be ready
  while(!apds.colorDataReady()){
    delay(5);
  }

  //get the data and print the different channels
  apds.getColorData(&r, &g, &b, &c);
  int _r = (float(r)/float(r+g+b))*255;
  int _g = (float(g)/float(r+g+b))*255;
  int _b = (float(b)/float(r+g+b))*255;
  lcd.setCursor(0,0);
  lcd.print("R: ");
  lcd.print(_r);
  
  lcd.print(" G: ");
  lcd.print(_g);
  lcd.setCursor(0,1);
  lcd.print(" B: ");
  lcd.print(_b);

  setRGB(255,255,0);
  delay(500);
}
