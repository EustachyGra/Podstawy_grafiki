#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

int czujnik = A1;   //pin analogowy A1 połączony z sygnałem z czujnika
float VOLT;
float TA, TC, H;
Adafruit_AM2320 am2320 = Adafruit_AM2320();
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup(){
    Serial.begin(9600);        //inicjalizacja monitora szeregowego
    lcd.init();
    lcd.backlight();
    while (!Serial) {
    delay(10); // hang out until serial port opens
    }
    Serial.println("Test czujników :D"); 
    am2320.begin();

}
     
void loop(){
    int odczyt = analogRead(czujnik);      //odczytanie wartości z czujnika
    VOLT = (odczyt * 5.0) / 1024.0;        //przeliczenie odczytanej wartości na napięcie w woltach (dla podłączenia pod 5 V)
    TA = VOLT  * 100;                    //konwersja z napięcia na temperaturę, rozdzielczość czujnika wynosi 10 mV na stopień
    TC = am2320.readTemperature();
    H = am2320.readHumidity();
    Serial.print("Ta: ");  
    Serial.print(TA);
    Serial.print(" Tc: "); 
    Serial.print(TC);
    Serial.print(" H: "); 
    Serial.println(H);

    lcd.setCursor(0,0);
    lcd.print("Ta:");
    lcd.print(TA);
    lcd.print(" Tc:");
    lcd.print(TC);

    lcd.setCursor(0,1);
    lcd.print("H:");
    lcd.print(H);
    lcd.print(" Hb:");
    float Hb = (H*TC)/100;
    lcd.print(Hb);

    delay(2000);                            //opóźnienie między kolejnymi odczytami
}