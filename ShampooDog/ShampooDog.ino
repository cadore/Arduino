//sensors input
#define SensorLevelEmergency A0
#define SensorLevelEmpty A1
#define SensorLevelMedium A2
#define SensorLevelHigh A3
#define SensorLevelShampooEmpty A4

//buttons input
#define ButtonEmergency 2

//actuators output
#define OutputWater 3
#define OutputShampoo 4
#define OutputEngine 5

//leds output
#define LedEmergency 6
#define LedShampooEmpty 7
#define LedOperation 8

//input variables
int LevelEmergency = 2;
int LevelEmpty = 2;
int LevelMedium = 2;
int LevelHigh = 2;
int LevelShampooEmpty = 2;
int BtnEmergency = 2;

//output variables
int WaterInput = 2;
int ShampoInput = 2;
int EngineInput = 2;

//times
long EmptyTime = 0;
long WaterProtectionTime = 0;
long ShampooProtectionTime = 0;
long ShakeTime = 0;
long ShakeBreakTime = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciado!");

  pinMode(SensorLevelEmergency, INPUT);
  pinMode(SensorLevelEmpty, INPUT);
  pinMode(SensorLevelMedium, INPUT);
  pinMode(SensorLevelHigh, INPUT);
  pinMode(SensorLevelShampooEmpty, INPUT);
  pinMode(ButtonEmergency, INPUT);

  pinMode(OutputWater, OUTPUT);
  pinMode(OutputShampoo, OUTPUT);
  pinMode(OutputEngine, OUTPUT);
  pinMode(LedEmergency, OUTPUT);
  pinMode(LedShampooEmpty, OUTPUT);
  pinMode(LedOperation, OUTPUT);
}

void loop() {
  ReadPorts();
  delay(10);

  if (LevelEmergency == HIGH) {
    digitalWrite(LedEmergency, HIGH);
  } else {
    digitalWrite(LedEmergency, LOW);

    if (LevelShampooEmpty == LOW) {
      digitalWrite(LedShampooEmpty, HIGH);
    } else {
      digitalWrite(LedShampooEmpty, LOW);
      ExecLoop();
    }
  }
}

void ExecLoop() {
  if (LevelEmpty == LOW) {
    if (EmptyTime == 0)
      EmptyTime = millis();

    if (millis() - EmptyTime >= 30000) {
      Water(1);
    }
  } else {
    ShakeTime = 0;
    if (LevelMedium == HIGH) {
      Water(0);
      delay(10);
    }

    if (LevelMedium == HIGH) {
      Shampoo(1);

      if (LevelHigh == HIGH) {
        Shampoo(0);
        delay(10);
      }
    }

    if (LevelMedium == HIGH && LevelHigh == HIGH) {
      Shake(1);
      if (ShakeTime == 0)
        ShakeTime = millis();

      if (millis() - ShakeTime >= 60000) {
        Shake(0);
      }
    }
  }
}

void ReadPorts() {
  LevelEmergency = digitalRead(SensorLevelEmergency);
  LevelEmpty = digitalRead(SensorLevelEmpty);
  LevelMedium = digitalRead(SensorLevelMedium);
  LevelHigh = digitalRead(SensorLevelHigh);
  LevelShampooEmpty = digitalRead(SensorLevelShampooEmpty);
  BtnEmergency = digitalRead(ButtonEmergency);

  WaterInput = digitalRead(OutputWater);
  ShampoInput = digitalRead(OutputShampoo);
  EngineInput = digitalRead(OutputEngine);
}

void Water(int i) {
  if (i == 0) {
    digitalWrite(OutputWater, LOW);
    Operation(0);
  } else {
    if (LevelEmpty == LOW) {
      digitalWrite(OutputWater, HIGH);
      Operation(1);
    }
  }
}

void Shampoo(int i) {
  if (i == 0) {
    digitalWrite(OutputShampoo, LOW);
    Operation(0);
  } else {
    if (LevelShampooEmpty == HIGH && LevelEmpty == HIGH && LevelMedium == HIGH) {
      digitalWrite(OutputWater, HIGH);
      Operation(1);
    }
  }
}

void Shake(int i) {
  if (i == 0) {
    digitalWrite(OutputEngine, LOW);
    Operation(0);
  } else {
    if (LevelEmpty == HIGH && LevelMedium == HIGH && LevelHigh == HIGH) {
      digitalWrite(OutputEngine, HIGH);
      Operation(1);
    }
  }
}

void Operation(int i) {
  if (i == 0)
    digitalWrite(LedOperation, LOW);
  else
    digitalWrite(LedOperation, HIGH);
}



