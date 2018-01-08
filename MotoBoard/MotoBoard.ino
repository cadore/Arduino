#include <LiquidCrystal.h>

#define interrupt_pin 2
#define ledStatusPin 10

volatile byte rpmcount = 0;
unsigned int rpm = 0, max_rpm = 0;
unsigned long timeoldrpm = 0;

int ledStatus = LOW;

LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

void setup()
{
	Serial.begin(9600);	
	startInt();
	initialize();
}
void loop()
{
	if (rpm > max_rpm) { max_rpm = rpm; }

	lcd.setCursor(0, 0);
	lcd.print("RPM: "); lcd.print(rpm, DEC);

	lcd.setCursor(0, 3);
	lcd.print("MAX: "); lcd.print(max_rpm, DEC);
	
	displayBar();
}

void initialize()
{
	pinMode(interrupt_pin, INPUT_PULLUP);

	lcd.begin(16, 4);
	lcd.setCursor(5, 1);
	lcd.print("CADORE");
	lcd.setCursor(3, 2);
	lcd.print("TECNOLOGIA");
}

void executeInt()
{
	if (rpmcount >= 20) {
		stopInt();
		rpm = ((60 * 1000) / ((millis() - timeoldrpm)*rpmcount));
		timeoldrpm = millis();
		rpmcount = 0;
		startInt();
	}
}

void rpm_fun()
{
	rpmcount++;
	if (ledStatus == LOW)

		ledStatus = HIGH;
	else
		ledStatus = LOW;
	digitalWrite(ledStatusPin, ledStatus);
}

void startInt()
{
	//CHANGE to trigger the interrupt whenever the pin changes value
	//RISING to trigger when the pin goes from low to high
	//FALLING for when the pin goes from high to low.
	attachInterrupt(digitalPinToInterrupt(interrupt_pin), rpm_fun, FALLING);
}

void stopInt()
{
	detachInterrupt(digitalPinToInterrupt(interrupt_pin));
}

void displayBar()
{
	int numOfBars = map(rpm, 0, max_rpm, 0, 15);
	lcd.setCursor(0, 1);
	if (rpm != 0)
	{
		for (int i = 0; i <= numOfBars; i++)
		{
			lcd.setCursor(i, 1);
			lcd.write(1023);
		}
	}
}