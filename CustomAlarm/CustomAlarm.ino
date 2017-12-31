#include <EEPROM.h>
#include <CTUtils.h>
#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PORT_SENSOR 4
#define WINDOW_SENSOR_1 5
#define WINDOW_SENSOR_2 6
#define PIR_SENSOR 7
#define LED_RED 8
#define LED_GREEN 9
#define ONEWIRE_PORT 10
#define BUZZER 11
#define ILLUMINATION_OUT 12

#define AUX_OUT_1 A0
#define AUX_OUT_2 A1

#define LDR_SENSOR A2
#define BUTTON_1 A3
#define BUTTON_2 A4
#define BUTTON_3 A5

OneWire oneWire(ONEWIRE_PORT);
DallasTemperature dallas_sensors(&oneWire);
DeviceAddress TemperatureSensor = { 0x10, 0x06, 0x1E, 0xE4, 0x02, 0x08, 0x00, 0xE9 }; //10 06 1E E4 02 08 00 E9
DeviceAddress iButtonTag = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

byte addr[8];

void setup() {
  Serial.begin(9600);
  Serial.println("Begin");
  Serial.println("Cadore Tecnologia");
}

void loop() {
  // put your main code here, to run repeatedly:

}

//valida tag, para saber se é iButton ou DS18b20
boolean consultaTagiButton() {
  boolean leituraok=false;
  if (oneWire.search(addr)) {
    if (OneWire::crc8(addr, 7) == addr[7]) { // testa a verificacao CRC
      if (addr[0]==1) {  // para tags iButton o primeiro byte sempre e' 1 
          leituraok=true;          
      } 
    }  
  }  
 oneWire.reset();
 return leituraok; 
}
