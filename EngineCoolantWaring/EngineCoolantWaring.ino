#include <CTUtils.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define LED_Y A0
#define LED_R A1
#define BUTTON 9
#define BUZZER 7
#define ONE_WIRE_BUS 8

enum ALARM
{
  NONE,
  WARING,
  DANGER
};

CTUtils ctu;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

DeviceAddress sensor_head = { 0x10, 0x06, 0x1E, 0xE4, 0x02, 0x08, 0x00, 0xE9 }; //10 06 1E E4 02 08 00 E9
DeviceAddress sensor_in = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
DeviceAddress sensor_out = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

// presets - setup
const long intervalUpDateTemperatures = 3000; //miliseconds
const long intervalUpDateDisplay = 5000; //miliseconds
double TempWaring = 87, TempDanger = 92;

unsigned long currentMillis = millis();
unsigned long lastUpDateTemperatures = 0;
unsigned long lastUpDateDisplay = 0;
double TempHead = 0, TempIn = 0, TempOut = 0;
double TempMinHead = 150, TempMaxHead = 0, TempMinIn = 150, TempMaxIn = 0, TempMinOut = 150, TempMaxOut = 0;
int CheckAlarmWaring, CheckAlarmDanger;
int ALARM = NONE;
int DISPLAYMINMAX = 0
bool BUZZER_MUTE = false;

void setup() {
  ctu.setLongPressTime(3000);
  loadEEPROM();
  initialize();
}

void loop() {
  CheckAlarmWaring = 0;
  CheckAlarmDanger = 0;
  currentMillis = millis();
  if (currentMillis - lastUpDateTemperatures >= intervalUpDateTemperatures) {
    lastUpDateTemperatures = currentMillis;
    updateTemperatures();
    verefyAlarms();
    writeEEPROM();
  }
  if (currentMillis - lastUpDateDisplay >= intervalUpDateDisplay) {
    lastUpDateDisplay = currentMillis;
    updateDisplay();
  }

  buttonActions();
}

void updateTemperatures(){
  sensors.requestTemperatures();
  TempHead = sensors.getTempC(sensor_head);
  TempIn = sensors.getTempC(sensor_in);
  TempOut = sensors.getTempC(sensor_out);

  if(TempHead > TempMaxHead) {TempMaxHead = TempHead;}
  if(TempHead < TempMinHead) {TempMinHead = TempHead;}
  
  if(TempIn > TempMaxIn) {TempMaxIn = TempIn;}
  if(TempIn < TempMinIn) {TempMinIn = TempIn;}
  
  if(TempOut > TempMaxOut) {TempMaxOut = TempOut;}
  if(TempOut < TempMinOut) {TempMinOut = TempOut;}  
}

void verefyAlarms(){
  // check alarms
  if(TempHead >= TempDanger || TempIn >= TempDanger || TempOut >= TempDanger)
    ALARM = DANGER;
  else if(TempHead >= TempWaring || TempIn >= TempWaring || TempOut >= TempWaring)
    ALARM = WARING;
  else
    ALARM = NONE;

  // alarm functions
  switch(ALARM){
    case DANGER:
      turnON(LED_R);
      if(!BUZZER_MUTE) turnON(BUZZER);
      CheckAlarmDanger = 1;
      break;

    case WARING:
      turnON(LED_Y);
      CheckAlarmWaring = 1;
      break;

    default:
    turnOFF(LED_Y);
    turnOFF(LED_R);
    turnOFF(BUZZER);
    BUZZER_MUTE = false;
  }
}

void updateDisplay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C: "); lcd.print(TempHead); lcd.write(B11011111); lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("E: "); lcd.print(TempIn); lcd.write(B11011111); lcd.print("C");
  lcd.setCursor(0, 2);
  lcd.print("S: "); lcd.print(TempOut); lcd.write(B11011111); lcd.print("C");

  lcd.setCursor(3, 0);
  switch(DISPLAYMINMAX){
    case 0:
      
      lcd.print("C: "); lcd.print(TempMinHead); lcd.print("/"); lcd.print(TempMaxHead);
      DISPLAYMINMAX = 1;
      break;
    
    case 1:
      lcd.print("E: "); lcd.print(TempMinIn); lcd.print("/"); lcd.print(TempMaxIn);
      DISPLAYMINMAX = 2;
      break;
      
    case 2:
      lcd.print("S: "); lcd.print(TempMinOut); lcd.print("/"); lcd.print(TempMaxOut);
      DISPLAYMINMAX = 0;
      break;
  }
  
}

void buttonActions(){
  int bb = ctu.getButtonLongPress(PIN_MENU);
  if (bb == 1) {
      BUZZER_MUTE = true;
      turnOFF(BUZZER);
  } else if (bb == 2) {
      writeEEPROM(true);
  }
}

void turnON(int p){
  digitalWrite(p, HIGH);
}
void turnOFF(int p){
  digitalWrite(p, LOW);
}

void initialize(){
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.setCursor(5, 1);
  lcd.print("CADORE");
  lcd.setCursor(1, 2);
  lcd.print("TECNOLOGIA");
  
  turnON(LED_Y);
  turnON(LED_R);
  turnON(BUZZER);
  delay(500);
  turnOFF(BUZZER);

  if(CheckAlarmWaring == 1){
    delay(700);
    turnON(BUZZER);
    delay(1000);
    turnOFF(BUZZER);
  }
  if(CheckAlarmDanger == 1){
    delay(700);
    turnON(BUZZER);
    delay(1000);
    turnOFF(BUZZER);
  }
  
}

void loadEEPROM(){
  TempMinHead = ctu.readFloatEEPROM(10);
  TempMaxHead = ctu.readFloatEEPROM(15);
  TempMinIn =  ctu.readFloatEEPROM(20);
  TempMaxIn = ctu.readFloatEEPROM(25);
  TempMinOut = ctu.readFloatEEPROM(30);
  TempMaxOut = ctu.readFloatEEPROM(35);
  CheckAlarmWaring = ctu.readFloatEEPROM(40);
  CheckAlarmDanger = ctu.readFloatEEPROM(45);
}

void writeEEPROM(bool resetmaxmin = false){
  if(resetmaxmin == true{
    ctu.writeFloatEEPROM(10, 00.00);
    ctu.writeFloatEEPROM(15, 00.00);
    ctu.writeFloatEEPROM(20, 00.00);
    ctu.writeFloatEEPROM(25, 00.00);
    ctu.writeFloatEEPROM(30, 00.00);
    ctu.writeFloatEEPROM(35, 00.00);
    ctu.writeFloatEEPROM(40, 0);
    ctu.writeFloatEEPROM(45, 0);
  }else{
    ctu.writeFloatEEPROM(10, TempMinHead);
    ctu.writeFloatEEPROM(15, TempMaxHead);
    ctu.writeFloatEEPROM(20, TempMinIn);
    ctu.writeFloatEEPROM(25, TempMaxIn);
    ctu.writeFloatEEPROM(30, TempMinOut);
    ctu.writeFloatEEPROM(35, TempMaxOut);
    ctu.writeFloatEEPROM(40, CheckAlarmWaring);
    ctu.writeFloatEEPROM(45, CheckAlarmDanger);
  }
}
