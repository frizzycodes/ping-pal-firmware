const int RED_PIN = 23;
const int GREEN_PIN =19;
const int BLUE_PIN = 18;
void setup() {
  // put your setup code here, to run once:
  pinMode(RED_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(BLUE_PIN,OUTPUT);
  Serial.begin(115200);
  Serial.println("Led test Started!");
}

void loop() {
  // put your main code here, to run repeatedly:
  setColor(255,0,0);
  delay(250);
  setColor(0,255,0);
  delay(250);
  setColor(0,0,255);
  delay(250);
}
void setColor(int r, int g, int b){
  analogWrite(RED_PIN,r);
  analogWrite(GREEN_PIN,g);
  analogWrite(BLUE_PIN,b);
}