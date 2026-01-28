#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
int czujnik = A1;   //pin analogowy A1 połączony z sygnałem z czujnika
float VOLT;
float TA, TC, H;
Adafruit_AM2320 am2320 = Adafruit_AM2320();

void setup(){
    Serial.begin(9600);        //inicjalizacja monitora szeregowego
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

    delay(2000);                            //opóźnienie między kolejnymi odczytami
}