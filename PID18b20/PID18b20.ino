#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <CTUtils.h>
#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//#define PIN_TempInternal A0
#define OUTPUT_PWM 3
#define DS_SENSOR 2
#define COMPRESSOR A2
#define ENGINE 4
#define IGNITION 5
#define BACKLIGHT 6
#define LIGHT 13

#define PIN_MINUS A5
#define PIN_PLUS A4
#define PIN_MENU A3

enum BUTTONS
{
  MINUS_TEMP,
  MINUS_CONFIG,
  PLUS_TEMP,
  PLUS_CONFIG,
  MENU
};

enum MENUS
{
  NORMAL,
  CONFIG
};

enum ONOFF
{
  ON,
  OFF
};

enum LightModes
{
  DAY,
  NIGHT
};


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(DS_SENSOR);
CTUtils ctu;
unsigned long lastUpDateDisplay = 0;
const long intervalUpDateDisplay = 500; //miliseconds
const long intervalTurnOnCompressor = 20; //secconds
const long intervalTurnOffBacklight = 15; //secconds
unsigned long currentMillis = millis();
int RealyOut = 0;
int engine_on;
int ignition;
long lastOffIgnition = 0;
int TypeMenu = NORMAL;
int StepMenu = 0;
int BackLightDay, BackLightNight, BackLightMode;

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress sensor_internal = { 0x10, 0x06, 0x1E, 0xE4, 0x02, 0x08, 0x00, 0xE9 }; //10 06 1E E4 02 08 00 E9
DeviceAddress sensor_external = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
DeviceAddress sensor_evaporator = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

//Define Variables we'll be connecting to
double Setpoint, TempInternal, Output, TempExternal, TempEvaporator;

//Specify the links and initial tuning parameters
double Kp = 37, Ki = 2, Kd = 1;
PID myPID(&TempInternal, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);

void setup()
{
  turnBacklight(ON);
  turnCompressor(LOW);
  Serial.begin(115200);
  Serial.println("Begin");
  lcd.begin(16, 4);
  sensors.begin();  
  Setpoint = ctu.readFloatEEPROM(2);
  BackLightDay = ctu.readFloatEEPROM(10);
  BackLightNight = ctu.readFloatEEPROM(20);
  Output = 0;
  TempInternal = 0;
  TempExternal = 0;
  TempEvaporator = 0;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  TCCR2B = TCCR2B & 0b11111000 | 0x01;
  //ctu.setLongPressTime(1500);
  lcd.setCursor(5, 1);
  lcd.print("CADORE");
  lcd.setCursor(-1, 2);
  lcd.print("TECNOLOGIA");
  delay(4000);
}

void loop()
{
  currentMillis = millis();
  if(digitalRead(LIGHT) == HIGH)
    BackLightMode = DAY;
  else
    BackLightMode = NIGHT;

  if (currentMillis - lastUpDateDisplay >= intervalUpDateDisplay) {
    // save the last time you blinked the LED
    lastUpDateDisplay = currentMillis;
    showMenuDisplay();
  }
  //verifica se motor esta ligado para ligar o compressor e o ventilador
  engine_on = digitalRead(ENGINE);
  ignition = digitalRead(IGNITION);
  int button = getChangeButtons(); 

  sensors.requestTemperatures();
  TempInternal = sensors.getTempC(sensor_internal);
  TempExternal = sensors.getTempC(sensor_external);
  TempEvaporator = 15;//sensors.getTempC(sensor_evaporator);

  if(ignition == HIGH){
    turnBacklight(ON);
    if (engine_on == LOW) {
      closeOutput();
    } 
    else {
      //Setpoint = map(analogRead(PIN_TempInternal), 0, 1023, 16, 31);
      if (button == MINUS_TEMP && Setpoint > 17) {
        Setpoint = (Setpoint - 0.50);
        ctu.writeFloatEEPROM(2, Setpoint);
      } 
      else if (button == PLUS_TEMP && Setpoint < 30.50) {
        Setpoint = (Setpoint + 0.50);
        ctu.writeFloatEEPROM(2, Setpoint);
      }
      myPID.Compute();
      if (TempEvaporator <= 10 || Setpoint == 30.50) {
        turnCompressor(LOW);
      } 
      else if (TempEvaporator >= 14 && Setpoint < 30.50) {
        turnCompressor(HIGH);
      }

      RealyOut = map(Output, 0, 255, 40, 230);
      if (RealyOut < 35 && Setpoint != 30.50) {
        RealyOut = 35;
      } 
      else if (Setpoint == 30.50) {
        RealyOut = 0;
      } 
      else if (Setpoint == 17) {
        RealyOut = 255;
      }

      analogWrite(OUTPUT_PWM, RealyOut);

      Serial.print(" ");
      Serial.print("SetPoint: ");
      if (Setpoint == 17) 
        Serial.print("LOW");
      else if (Setpoint == 30.50) 
        Serial.print("HIGH");
      else
        Serial.print(Setpoint);
      Serial.print(" ");
      Serial.print("Temp: ");
      Serial.print(TempInternal);
      Serial.print(" ");
      Serial.print(" Output: ");
      Serial.print(Output);
      Serial.print(" / ");
      Serial.print(RealyOut);
    }
    int bb = ctu.getButtonLongPress(PIN_MENU);
    if (bb == 1) {
      nextStepMenu();
    } 
    else if (bb == 2) {
      if(TypeMenu == NORMAL){
        TypeMenu = CONFIG;
        StepMenu = 0;
      }
      else if(TypeMenu == CONFIG){
        TypeMenu = NORMAL;
        StepMenu = 0;
      }
    }
  }
  else{
    closeOutput();
    TypeMenu = NORMAL;
    StepMenu = 0;
    if(lastOffIgnition == 0)
      lastOffIgnition = currentMillis;

    if((currentMillis - lastOffIgnition) >= (intervalTurnOffBacklight * 1000)){
      turnBacklight(OFF);
      lastOffIgnition = 0;
    }
  }
  Serial.println("");
  Serial.print(" Engine:");
  Serial.print(engine_on == LOW ? "OFF" : "ON");
  Serial.print(" Ignition:");
  Serial.print(ignition == LOW ? "OFF" : "ON");
  Serial.println("\n");
  Serial.println(StepMenu);

  if(TypeMenu == NORMAL)
    Serial.println("NORMAL");
  else
    Serial.println("CONFIG");  
}

void closeOutput(){
  Output = 0;
  analogWrite(OUTPUT_PWM, 0);
  turnCompressor(LOW);
}

int getChangeButtons() {
  //Serial.println(digitalRead(PIN_MINUS));
  //Serial.println(digitalRead(PIN_PLUS));

  if (digitalRead(PIN_MINUS) == HIGH)
    return TypeMenu == NORMAL ? MINUS_TEMP : MINUS_CONFIG;
  else if (digitalRead(PIN_PLUS) == HIGH)
    return TypeMenu == NORMAL ? PLUS_TEMP : PLUS_CONFIG;
  else if (digitalRead(PIN_MENU) == HIGH)
    return MENU;
  else
    return 1023;
}

void showMenuDisplay() {
  lcd.clear();
  if(TypeMenu == NORMAL){
    lcd.setCursor(0, 0);
    lcd.print(" Interna:");
    lcd.setCursor(10, 0);
    lcd.print(TempInternal);

    lcd.setCursor(0, 1);
    lcd.print(" Externa:");
    lcd.setCursor(10, 1);
    lcd.print(TempExternal + 127 + 32);

    if (engine_on == HIGH && ignition == HIGH) {
      lcd.setCursor(-4, 2);
      lcd.print(" Ajuste:");
      lcd.setCursor(6, 2);
      if (Setpoint == 17) 
        lcd.print("LOW");
      else if (Setpoint == 30.50) 
        lcd.print("HIGH");
      else
        lcd.print(Setpoint);

      lcd.setCursor(-4, 3);
      lcd.print(" Vent.:");
      lcd.setCursor(6, 3);
      lcd.print(map(RealyOut, 0, 255, 0, 100));
      lcd.print("%");
    }
  }
  else if(TypeMenu == CONFIG){
    lcd.setCursor(0, 0);
    lcd.print(">Configuracoes");
    lcd.setCursor(2, 1);
    lcd.print("Luz de fundo");

    if(StepMenu == 0){      
      lcd.setCursor(0, 2);
      lcd.print("Modo DIA");
    }
    else if(StepMenu == 1){      
      lcd.setCursor(-1, 2);
      lcd.print("Modo NOITE");
    }

  }
}

long lastOffCompressor = 0;
void turnCompressor(int i){
  if(i == HIGH){
    if((currentMillis - lastOffCompressor) >= (intervalTurnOnCompressor * 1000)){
      digitalWrite(COMPRESSOR, HIGH);
      lastOffCompressor = 0;
    }
  }
  else if(i == LOW){
    if(lastOffCompressor == 0)
      lastOffCompressor = millis();
    digitalWrite(COMPRESSOR, LOW);    
  }
}

void turnBacklight(int i){
  if(i == ON){
    if(BackLightMode == DAY)
      analogWrite(BACKLIGHT, BackLightDay);
    else if(BackLightMode == NIGHT)
      analogWrite(BACKLIGHT, BackLightNight);
  }
  else if (i == OFF){
    analogWrite(BACKLIGHT, 0);
  }
}

int TotalStepNormal = 2;
int TotalStepConfig = 2;
void nextStepMenu(){
  if(TypeMenu == NORMAL){
    StepMenu++;
    if(StepMenu > (TotalStepNormal - 1)){
      StepMenu = 0;
    }
  }
  else{
    StepMenu++;
    if(StepMenu > (TotalStepConfig - 1)){
      StepMenu = 0;
    }
  }
}


