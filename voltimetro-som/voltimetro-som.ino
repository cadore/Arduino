#define BAT_VIN A0
#define BAT_AUX_IN A1
#define REM_IN 2
#define REM_OUT 3
#define REM_AUX 4
#define BAT_AUX A2
#define BUZZER 5


void setup() {
  PinMode();
}

void loop() {
  Volt();
}

void Volt(){
  float Vin = analogRead(BAT_VIN);
  float BatAux = analogRead(BAT_AUX_IN);
  
}

void PinMode(){
   pinMode(REM_IN, INPUT);
   pinMode(REM_OUT, OUTPUT);
   pinMode(REM_AUX, OUTPUT);
   pinMode(BAT_AUX, OUTPUT);
   pinMode(BUZZER, OUTPUT);

   digitalWrite(REM_OUT, LOW);
   digitalWrite(REM_AUX, LOW);
   digitalWrite(BAT_AUX, LOW);
   digitalWrite(BUZZER, LOW);
}

