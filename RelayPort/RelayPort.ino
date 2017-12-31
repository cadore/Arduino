#define IGNITION 2
#define PORT 4
#define OUT 10

unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  //pinMode(IGNITION, INPUT);
  //pinMode(PORT, INPUT);
  pinMode(OUT, OUTPUT);
  digitalWrite(OUT, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  int ign = digitalRead(IGNITION);
  int pot = digitalRead(PORT);

  if(pot == HIGH)
  {    
    digitalWrite(OUT, HIGH);
  }
  else if(pot == LOW)
  {    
    if(ign == HIGH)
    {
      digitalWrite(OUT, LOW);
    }
    else if(ign == LOW)
    {      
      if (currentMillis - previousMillis >= interval) 
      {
        previousMillis = currentMillis;
        digitalWrite(OUT, LOW);
      }            
    } 
  }
  delay(10);
}
