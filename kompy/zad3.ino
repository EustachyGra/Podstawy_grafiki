int r,g,b;

void setup(){
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);   
  pinMode(9, OUTPUT); 
}

void setRGB()
{
    analogWrite(11,r);
    analogWrite(10,g);
    analogWrite(9,b);
}

void randRGB()
{

    r = random(0,255);
    b = random(0,255);
    g = random(0,255);

}
void loop(){
    delay(200);
    randRGB();
    setRGB();
}