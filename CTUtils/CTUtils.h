#ifndef CTUtils_h
#define CTUtils_h

#include "Arduino.h"

class CTUtils
{
  public:
    CTUtils();
	void setLongPressTime(int longPressTimeB);
    int getButtonLongPress(int button);
	void writeFloatEEPROM(int address, float value);
	float readFloatEEPROM(int address);
};
#endif