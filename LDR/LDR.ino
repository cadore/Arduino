#define SENSOR A0
#define OUT 13

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SENSOR, INPUT);
  pinMode(OUT, OUTPUT);
}

void loop() {
  int valorSensor = analogRead(SENSOR);
  if(valorSensor >= 750)
  {
    digitalWrite(OUT, LOW);
  }else{
    digitalWrite(OUT, HIGH);
  }
  
  Serial.println(valorSensor);
  delay(100);
}
