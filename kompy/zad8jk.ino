int sound_din=2;
int sound_ain=A0;
int led_dout = 13;
int ad_value;
void setup()
{
  pinMode(sound_din,INPUT);
  pinMode(sound_ain,INPUT);
  pinMode(led_dout,OUTPUT);

  Serial.begin(9600);
}
void loop()
{
  ad_value=analogRead(sound_ain);
  Serial.println(ad_value);
  if(ad_value>700)
  {
    digitalWrite(led_dout,HIGH);
    delay(2000);
  }
  else
  {
      digitalWrite(led_dout,LOW);
  }
}
