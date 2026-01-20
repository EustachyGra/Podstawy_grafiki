#include "RTC.h"
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_LED 12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

uint16_t minuta = 0, godzina = 0; 
volatile bool write = false;

void setup() {

  Serial.begin(9600);
  RTC.begin();
  RTCTime startTime(30, Month::JUNE, 2023, 13, 37, 00, DayOfWeek::WEDNESDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTime(startTime);
  if (!RTC.setPeriodicCallback(periodicCallback, Period::ONCE_EVERY_2_SEC)) {

    Serial.println("ERROR: periodic callback not set");

  }
  strip.begin();
  strip.setBrightness(5);
  strip.show();
}


void loop() {
  if(write){
    setPixel(minuta,strip.Color(0, 0, 0));
    minuta++;
    if(minuta%12==0)
    {
      setPixel(godzina,strip.Color(0, 0, 0));
      godzina++;
    }
    minuta=minuta%12;
    godzina=godzina%12;
    if(minuta!=godzina){
    setPixel(minuta,strip.Color(255, 0, 0));
    setPixel(godzina,strip.Color(0, 255, 0));
    }
    else
    {
      setPixel(minuta,strip.Color(0, 0, 255));
    }
    Serial.println(strip.numPixels());
    write = false;
  }
}

void periodicCallback()
{
  write = true;
}

void setPixel(uint16_t i, uint32_t c) {
    
      strip.setPixelColor(i, c);
      strip.show();
    
}
