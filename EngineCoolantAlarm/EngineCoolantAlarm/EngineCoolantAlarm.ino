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
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

DeviceAddress sensor_head = { 0x10, 0x57, 0x02, 0xF7, 0x02, 0x08, 0x00, 0x1D }; //10 57 02 F7 02 08 00 1D / 10 78 68 DE 02 08 00 C0 the original system
DeviceAddress sensor_in = { 0x10, 0xDF, 0xDA, 0x0D, 0x03, 0x08, 0x00, 0xF7 }; // 10 DF DA 0D 03 08 00 F7
DeviceAddress sensor_out = { 0x10, 0xFA, 0xF5, 0xE3, 0x02, 0x08, 0x00, 0xCA }; // 10 FA F5 E3 02 08 00 CA

// presets - setup
const long intervalUpDateTemperatures = 2000; //miliseconds
const long intervalUpDateDisplay = 3000; //miliseconds
double TempWaring = 87, TempDanger = 92;

unsigned long currentMillis = millis();
unsigned long startMillis = millis();
unsigned long lastUpDateTemperatures = 0;
unsigned long lastUpDateDisplay = 0;
double TempHead, TempIn, TempOut;
double TempMinHead = 150, TempMaxHead = 0, TempMinIn = 150, TempMaxIn = 0, TempMinOut = 150, TempMaxOut = 0;
int CheckAlarmWaring, CheckAlarmDanger;
int ALARM = NONE;
int DISPLAYMINMAX = 0;
bool BUZZER_MUTE = false;

void setup() {
  Serial.begin(9600);
  ctu.setLongPressTime(3000);
  loadEEPROM();
  initialize();
}

void loop() {
  currentMillis = millis();
  if (currentMillis - lastUpDateTemperatures >= intervalUpDateTemperatures) {
    lastUpDateTemperatures = currentMillis;
    updateTemperatures();
    writeEEPROM(false);
  }
  if (currentMillis - lastUpDateDisplay >= intervalUpDateDisplay) {
    lastUpDateDisplay = currentMillis;
    updateDisplay();
  }
  verefyAlarms();
  buttonActions();
}

void updateTemperatures(){
  sensors.requestTemperatures();
  TempHead = sensors.getTempC(sensor_head);
  TempIn = sensors.getTempC(sensor_in);
  TempOut = sensors.getTempC(sensor_out);

  if(millis() - startMillis >= 1000){
    if(TempHead > TempMaxHead) {TempMaxHead = TempHead;}
    if(TempHead < TempMinHead) {TempMinHead = TempHead;}
    
    if(TempIn > TempMaxIn) {TempMaxIn = TempIn;}
    if(TempIn < TempMinIn) {TempMinIn = TempIn;}
    
    if(TempOut > TempMaxOut) {TempMaxOut = TempOut;}
    if(TempOut < TempMinOut) {TempMinOut = TempOut;}  
  }
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
  writeEEPROM(false);
}

void updateDisplay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C: "); lcd.print(TempHead); lcd.write(B11011111); lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("E: "); lcd.print(TempIn); lcd.write(B11011111); lcd.print("C");
  lcd.setCursor(0, 2);
  lcd.print("S: "); lcd.print(TempOut); lcd.write(B11011111); lcd.print("C");

  lcd.setCursor(0, 3);
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
  int bb = ctu.getButtonLongPress(BUTTON);
  if (bb == 1) {
      BUZZER_MUTE = true;
      turnOFF(BUZZER);
      Serial.println("short");
  } else if (bb == 2) {
      writeEEPROM(true);
      turnON(BUZZER);
      
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("CADORE");
      lcd.setCursor(3, 2);
      lcd.print("TECNOLOGIA");
      lcd.setCursor(3, 3);
      lcd.print("ZERANDO...");      
      delay(2000);
      turnOFF(BUZZER);
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

  sensors.begin();
  sensors.setResolution(sensor_head, 9);
  sensors.setResolution(sensor_in, 9);
  sensors.setResolution(sensor_out, 9);
  
  lcd.begin(16, 4);

  lcd.setCursor(5, 1);
  lcd.print("CADORE");
  lcd.setCursor(3, 2);
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
  float fTempMinHead = ctu.readFloatEEPROM(10);
  float fTempMaxHead = ctu.readFloatEEPROM(15);
  float fTempMinIn =  ctu.readFloatEEPROM(20);
  float fTempMaxIn = ctu.readFloatEEPROM(25);
  float fTempMinOut = ctu.readFloatEEPROM(30);
  float fTempMaxOut = ctu.readFloatEEPROM(35);

  if(fTempMinHead == 0) {} else{ TempMinHead = fTempMinHead; }
  if(fTempMaxHead == 0) {} else{ TempMaxHead = fTempMaxHead; }
  if(fTempMinIn == 0) {} else{ TempMinIn = fTempMinIn; }
  if(fTempMaxIn == 0) {} else{ TempMaxIn = fTempMaxIn; }
  if(fTempMinOut == 0) {} else{ TempMinOut = fTempMinOut; }
  if(fTempMaxOut == 0) {} else{ TempMaxOut = fTempMaxOut; }
  
  CheckAlarmWaring = ctu.readFloatEEPROM(40);
  CheckAlarmDanger = ctu.readFloatEEPROM(45);
}

void writeEEPROM(bool resetmaxmin){
  if(resetmaxmin == true){
    ctu.writeFloatEEPROM(10, 0);
    ctu.writeFloatEEPROM(15, 0);
    ctu.writeFloatEEPROM(20, 0);
    ctu.writeFloatEEPROM(25, 0);
    ctu.writeFloatEEPROM(30, 0);
    ctu.writeFloatEEPROM(35, 0);
    ctu.writeFloatEEPROM(40, 0);
    ctu.writeFloatEEPROM(45, 0);

    TempMinHead = TempHead;
    TempMaxHead = TempHead;
    TempMinIn =  TempIn;
    TempMaxIn = TempIn;
    TempMinOut = TempOut;
    TempMaxOut = TempOut; 
    CheckAlarmWaring = 0;
    CheckAlarmDanger = 0;
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
