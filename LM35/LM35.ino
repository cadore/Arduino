const int LM35 = A0;
const int LDR = A1;
const int LED = 2;
float temperatura;
int luz;

void setup() {
  Serial.begin(9600);
  pinMode(LM35, INPUT);
  pinMode(LDR, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  temperatura = (float(analogRead(LM35)) * 5 / (1023)) / 0.01;
  Serial.print("Temperatura: ");
  Serial.println(temperatura);

  luz = analogRead(LDR);
  if(luz > 700){
    digitalWrite(LED, LOW);
  }else{
    digitalWrite(LED, HIGH); 
  }
  Serial.println(luz);
  
  delay(1200);
}
