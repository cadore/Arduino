#include <RFremote.h>
#define CONTROL_BUTTON 3
#define SENSOR_PORT 4
#define SENSOR_WINDOW 5
#define SIREN 6
#define BLUE_LED 7
#define RED_LED 8
#define OUT_AUX 10
#define INDEFINIDO -1

unsigned long previousMillisLedBlink = 0;
unsigned long previousMillisWaitFor = 0;
unsigned long previousMillisShowWaitFor = 0;
int state;

enum Status {
  ALARM_OFF,
  ALARM_ON,
  ALARM_STARTED
};

enum receivedSignal {
  CONTROL_SIGNAL,
  SENSOR_SIGNAL
};

//botao 1 - 0110100100100100100110110110100100100100110100110100110100100110110100110100110100110
//botao 2 - 0110100100100100100110110110100100100100110100110100110100100110110110100100110100110
//botao 3 - 0110100100100100100110110110100100100100110100110100110100100110110110110100110100110
const char *controle = "0110100100100100100110110110100100100100110100110100110100100110110110110100110100110";


SignalPatternParams params;
RFrecv rfrecv;

void setup() {
  initiatePins();
  state = ALARM_ON;
  sirenBeep(200);
  Serial.begin(9600);
  params.spaceMin = 11000;
  params.spaceMax = 13000;
  params.dotMin = 300;
  params.dotMax = 600;
  params.traceMin = 700;
  params.traceMax = 1050;
  params.skipFirst = 0;
  params.skipLast = 0;

  rfrecv = RFrecv(&params);
  rfrecv.begin();

  //Serial.println("CADORE TECNOLOGIA");
  Serial.println("System Started");
  Serial.println("Alarm is ON");
}

void loop() {
  int signalReceived = receivedSignal();
  switch (state) {
    case ALARM_OFF:
      if (getVialationPortSensors()) {
        turnAux(1);
      }
      if (signalReceived == CONTROL_SIGNAL) {
        Serial.println("Alarm is starting in 15 seconds");
        turnAux(0);
        turnOff(BLUE_LED);
        state = ALARM_ON;
        sirenBeep(200);
        waitFor(BLUE_LED, 15000, ALARM_OFF);
        if (state == ALARM_ON)
          Serial.println("Alarm started");
        else {
          Serial.println("Alarm starting canceled");
          Serial.println("Alarm Return Standby");
        }
        turnOff(BLUE_LED);
        sirenBeep(80);
        break;
      }
      ledBlink(BLUE_LED, 1000);
      turnOff(RED_LED);
      break;
    case ALARM_ON:
      turnAux(0);
      if (signalReceived == CONTROL_SIGNAL) {
        state = ALARM_OFF;
        turnOff(RED_LED);
        sirenBeep(200);
        delay(200);
        sirenBeep(200);
        turnOff(RED_LED);
        Serial.println("Alarm is Standby");
        break;
      } else if (signalReceived == SENSOR_SIGNAL) {
        Serial.println("Alarm is Fired in 5 seconds");
        state = ALARM_STARTED;
        waitFor(RED_LED, 5000, ALARM_OFF);
        if (state == ALARM_STARTED) {
          Serial.println("Alarm Fired");
          turnOn(SIREN);
        } else {
          Serial.println("Alarm Fired Canceled");
          Serial.println("Alarm is Standby");
        }        
      }
      ledBlink(RED_LED, 700);
      break;
    case ALARM_STARTED:
      turnAux(0);
      if (signalReceived == CONTROL_SIGNAL) {
        turnOff(SIREN);
        state = ALARM_ON;
        delay(600);
        turnOn(RED_LED);
        turnOn(SIREN);
        delay(600);
        turnOff(SIREN);
        turnOff(RED_LED);
        Serial.println("Alarm Fired Canceled");
        Serial.println("Alarm ON");
        break;
      }
      ledBlink(RED_LED, 200);
      break;
  }
}

void initiatePins() {
  pinMode(2, INPUT);
  pinMode(CONTROL_BUTTON, INPUT);
  pinMode(SENSOR_PORT, INPUT);
  pinMode(SENSOR_WINDOW, INPUT);
  pinMode(SIREN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(OUT_AUX, OUTPUT);
}

int receivedSignal() {
  if (rfrecv.available()) {
    //Serial.println((char*)rfrecv.cmd);
    //Serial.println();
    if (strncmp((char*)rfrecv.cmd, controle, CMD_SIZE) == 0) {
      Serial.println("Control Signal");
      return CONTROL_SIGNAL;
    }
  }
  if (digitalRead(CONTROL_BUTTON) == 0) {
    delay(100);
    Serial.println("Button Signal");
    return CONTROL_SIGNAL;
  }
  if (getVialationSensors()) {
    return SENSOR_SIGNAL;
  }  
  return INDEFINIDO;
}

void ledBlink(int led, int speed_milis) {
  int state = digitalRead(led);
  const long interval = speed_milis;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisLedBlink >= interval) {
    previousMillisLedBlink = currentMillis;
    digitalWrite(led, !state);
  }
}

void turnOn(int pin) {
  digitalWrite(pin, HIGH);
}

void turnOff(int pin) {
  digitalWrite(pin, LOW);
}

void turnAux(int i) {
  if (i == 0) {
    turnOff(OUT_AUX);
    //Serial.println("Auxiliar Out is OFF");
  }
  else if (i == 1) {
    turnOn(OUT_AUX);
    //Serial.println("Auxiliar Out is ON");
  }
}

void sirenBeep(int mi) {
  turnOn(SIREN);
  delay(mi);
  turnOff(SIREN);
}

void waitFor(int led, int time_skip, int sts) {
  const long interval = time_skip;
  previousMillisWaitFor = millis();
  long t = (time_skip / 1000);
  bool endd = false;
  while (true) {
    ledBlink(led, 50);
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillisWaitFor >= interval)) {
      previousMillisWaitFor = currentMillis;
      break;
    } else if (receivedSignal() == CONTROL_SIGNAL) {
      state = sts;
      sirenBeep(200);
      delay(250);
      sirenBeep(200);
      break;
    }
    unsigned long currentMillisS = millis();
    if ((currentMillisS - previousMillisShowWaitFor >= 1000)) {
      previousMillisShowWaitFor = currentMillisS;
      if (t > 0) {
        Serial.print(t); Serial.print("... ");
      }
      t = t - 1;
    }
    if (t == 0 && endd == false) {
      Serial.println("");
      endd = true;
    }
  }
}

bool getVialationSensors() {
  if (!getVialationWindowSensors() && !getVialationPortSensors()) {
    return false;
  } else {
    return true;
  }
}

bool getVialationWindowSensors() {
  if (digitalRead(SENSOR_WINDOW) == 0) {
    return false;
  } else {
    if (state == ALARM_ON)
      Serial.println("Sensor Window Infringed");
    return true;
  }
}

bool getVialationPortSensors() {
  if (digitalRead(SENSOR_PORT) == 0) {
    return false;
  } else {
    if (state == ALARM_ON)
      Serial.println("Sensor Port Infringed");
    return true;
  }
}

