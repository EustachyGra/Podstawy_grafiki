#include <Wire.h>
#include <LPS.h>
#include <LiquidCrystal_I2C.h>

LPS ps;
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (!ps.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");
    while (1);
  }
  lcd.init();
  
  lcd.backlight();
  lcd.setCursor(0,0);

  ps.enableDefault();
}

void loop()
{
  lcd.clear();
  lcd.setCursor(0,0);
  float pressure = ps.readPressureMillibars();
  float altitude = ps.pressureToAltitudeMeters(pressure);
  
  Serial.print("p: ");
  Serial.print(pressure);
  Serial.print(" mbar a: ");
  Serial.print(altitude);
  Serial.println(" m: ");

  lcd.print("p: ");
  lcd.print(pressure);
  lcd.print(" mbar");
  lcd.setCursor(0,1);
  lcd.print("a: ");
  lcd.print(altitude);

  delay(100);
}
