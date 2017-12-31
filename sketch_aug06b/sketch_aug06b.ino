#include <PID_v1.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_INPUT A0
#define PIN_OUTPUT 9
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);

void setup()
{
  Serial.begin(9600);
  Serial.println("Begin");

  // Start up the library
  sensors.begin();
  //initialize the variables we're linked to
  Input = 0;
  Setpoint = 100;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop()
{
  Setpoint = map(analogRead(PIN_INPUT), 0, 1023, 16, 25);
  sensors.requestTemperatures(); // Send the command to get temperatures
  Input = sensors.getTempCByIndex(0);
  myPID.Compute();
  digitalWrite(PIN_OUTPUT, Output);
  
  Serial.println("");
  Serial.println("SetPoint: ");
  Serial.print(Setpoint);
  Serial.println("");
  Serial.println("Input: ");
  Serial.print(Input);
  Serial.println("");
  Serial.println("Output: ");
  Serial.print(Output);
  delay(1000);
}


