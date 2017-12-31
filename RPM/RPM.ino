// read RPM and calculate average every then readings.
const int numreadings = 10;
int readings[numreadings];
unsigned long average = 0;
int index = 0;
unsigned long total;

volatile int rpmcount = 0;//see http://arduino.cc/en/Reference/Volatile
unsigned long rpm = 0;
unsigned long lastmillis = 0;

void setup() {
  Serial.begin(9600);
  attachInterrupt(0, rpm_fan, FALLING);
  delay(2000);
  Serial.println("Conta giros");
}

void loop() {
  if (millis() - lastmillis >= 100) { /*Uptade every one second, this will be equal to reading frecuency (Hz).*/

    detachInterrupt(0);    //Disable interrupt when calculating
    total = 0;
    readings[index] = rpmcount * 300;  /* Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use rpmcount * 30.*/

    for (int x = 0; x <= 9; x++) {
      total = total + readings[x];
    }

    average = total / numreadings;
    rpm = average;

    rpmcount = 0; // Restart the RPM counter
    index++;
    if (index >= numreadings) {
      index = 0;
    }


    if (millis() > 100) { // wait for RPMs average to get stable

      Serial.print(" RPM = ");
      Serial.println(rpm);
    }

    lastmillis = millis(); // Uptade lasmillis
    attachInterrupt(0, rpm_fan, FALLING); //enable interrupt
  }
}


void rpm_fan() { /* this code will be executed every time the interrupt 0 (pin2) gets low.*/
  rpmcount++;
}




