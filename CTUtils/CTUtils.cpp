#include "Arduino.h"
#include "CTUtils.h"
#include "EEPROM.h"

long buttonTimer = 0;
long longPressTime = 300;

boolean buttonActive = false;
boolean longPressActive = false;

CTUtils::CTUtils() {
}

void CTUtils::setLongPressTime(int longPressTimeB){
	longPressTime = longPressTimeB;
}

int CTUtils::getButtonLongPress(int button) {
  int _return = 0;
  button = digitalRead(button);
  if (button == HIGH) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      _return = 2;
    }
  } else if (button == LOW){
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        _return = 1;
      }
      buttonActive = false;
    }
  }
  return _return;
}

//EEPROM
//union  value
typedef union {
  float flt;
  byte array[4];
} FloatConverter;

//write
void CTUtils::writeFloatEEPROM(int address, float value)
{
  FloatConverter aConverter; //create a new variable of type FloatConverter
  aConverter.flt = value; //set its value (using the float blueprint) to the value of config
  for (byte i = 0; i < 4; i++) {
    EEPROM.write(address + i, aConverter.array[i]); //store each of the 4 bytes of aConverter to the EEPROM, accessing them using the byte[4] blueprint
    //Serial.print(aConverter.array[i]);
  };
  //Serial.println();
}


//read
float CTUtils::readFloatEEPROM(int address)
{
  float value;
  FloatConverter aConverter; //create a new variable of type FloatConverter
  for (byte i = 0; i < 4; i++) {
    aConverter.array[i] = EEPROM.read(address + i); //read 4 bytes from the EEPROM to aConverter using the byte[4] blueprint
  }
  value = aConverter.flt; //set the value of config to the value of aConverter using the float blueprint}
  //Serial.println(value);
  return value;
}
//end EEPROM