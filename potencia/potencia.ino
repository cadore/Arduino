int temp1; //ponte retificadora
const int tempPin1 = A0;

int temp2; //Canal A
const int tempPin2 = A1;

int protecao;
int var;
void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(9, OUTPUT);
  pinMode(13, OUTPUT);
  analogWrite(3, 210);// LED IPC
  analogWrite(6, 210);
  digitalWrite(9, LOW);// POWER
  delay(1000);
  analogWrite(3, 0);
  analogWrite(6, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  temp1 = (5.0 * analogRead(tempPin1) * 100.0) / 1024;
  temp2 = (5.0 * analogRead(tempPin2) * 100.0) / 1024;
  if (temp1 > 85) {
    digitalWrite(9, LOW);
    analogWrite(3, 210);
    analogWrite(5, 0);
    analogWrite(6, 0);
    delay(60000);
  } else {
    if (temp1 < 80) {
      digitalWrite(9, HIGH);
      //digitalWrite(2, LOW);
      analogWrite(5, 155);
      //digitalWrite(4, LOW);
    }
  }
  if (temp2 > 85) {
    digitalWrite(9, LOW);
    analogWrite(3, 0);
    analogWrite(5, 0);
    analogWrite(6, 210);
    delay(60000);
  } else {
    if (temp2 < 80) {
      digitalWrite(9, HIGH);
      //digitalWrite(2, LOW);
      analogWrite(5, 155);
      //digitalWrite(4, LOW);
    }
  }

  delay(1000);
}

