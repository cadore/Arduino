#define IGNITION_IN 0
#define PORT_IN 1
#define START_IN 2

#define PORT_OUT 3
#define FUEL_OUT 4

long IgnitionLife = 0;
long millisPort = 0;
long millisWait = 0;
int ENG = LOW;

void setup() {
  PinMode();
}

void loop() {
  int IGN = digitalRead(IGNITION_IN);
  int POR = digitalRead(PORT_IN);
  //int ENG = digitalRead(START_IN);
  
  if(ENG == LOW){
    if(digitalRead(START_IN) == HIGH){
      ENG = HIGH;
    }else{
      ENG = LOW;
    }
  }
  if (IGN == HIGH) {
    if (IgnitionLife == 0)
      IgnitionLife = millis();
    if (ENG == HIGH) {
      digitalWrite(FUEL_OUT, HIGH);
    }
    else {
      if (millis() - IgnitionLife >= 5000) {
        digitalWrite(FUEL_OUT, LOW);
      }
      else {
        digitalWrite(FUEL_OUT, HIGH);
      }
    }
    digitalWrite(PORT_OUT, LOW);
  }
  else
  {
    IgnitionLife = 0;
    digitalWrite(FUEL_OUT, LOW);
    ENG = LOW;
  }

  if (POR == LOW) {
    millisPort = 0;
    if (millisWait == 0)
      millisWait = millis();

    if (millis() - millisWait >= 420000)
      digitalWrite(PORT_OUT, LOW);
    else
      digitalWrite(PORT_OUT, HIGH);

  }
  else {
    millisWait = 0;
    if (millisPort == 0)
      millisPort = millis();

    if (IGN == LOW) {
      if (millis() - millisPort >= 6000) {
        digitalWrite(PORT_OUT, LOW);
      }
    }
    else {
      digitalWrite(PORT_OUT, LOW);
    }
  }
  delay(30);
}

void PinMode() {
  pinMode(IGNITION_IN, INPUT);
  pinMode(PORT_IN, INPUT);
  pinMode(START_IN, INPUT);
  pinMode(PORT_OUT, OUTPUT);
  pinMode(FUEL_OUT, OUTPUT);
}


