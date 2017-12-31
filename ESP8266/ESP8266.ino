#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define LED 9
 
//RX pino 2, TX pino 3
SoftwareSerial esp8266(2, 3);
 
#define DEBUG true
 // Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS 8
 
// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);
 
// Armazena temperaturas minima e maxima
float tempMin = 999;
float tempMax = 0;
 
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;
void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(115200);  
  //sendData("AT+UART_DEF=9600,8,1,0,0", 2000, DEBUG); // rst
  sendData("AT+RST\r\n", 2000, DEBUG); // rst
  // Conecta a rede wireless
  sendData("AT+CWJAP=\"CT-PRINCIPAL\",\"nao sabemos a senha\"\r\n", 2000, DEBUG);
  delay(3000);
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  // Mostra o endereco IP
  sendData("AT+CIFSR\r\n", 1000, DEBUG);
  // Configura para multiplas conexoes
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
  // Inicia o web server na porta 80
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);

  //ds18b20
  sensors.begin();
  // Localiza e mostra enderecos dos sensores
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0)) 
     Serial.println("Sensores nao encontrados !"); 
  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensor: ");
  mostra_endereco_sensor(sensor1);
}

void mostra_endereco_sensor(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void loop()
{
// Le a informacao do sensor
  /*sensors.requestTemperatures();
  float tempC = sensors.getTempC(sensor1);
  // Atualiza temperaturas minima e maxima
  if (tempC < tempMin)
  {
    tempMin = tempC;
  }
  if (tempC > tempMax)
  {
    tempMax = tempC;
  }*/
  
  // Verifica se o ESP8266 esta enviando dados
  if (esp8266.available())
  {
    if (esp8266.find("+IPD,"))
    {
      //delay(200);
      int connectionId = esp8266.read() - 48;
 
      String webpage = "<head><meta http-equiv=""refresh"" content=""10"">";
      webpage += "<title>ESP8266 - Cadore Tecnologia</title>";
      webpage += "</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";
      webpage += "Analogica A0: ";
      int a = analogRead(A0);
      webpage += a;
      webpage += "<h2>Porta Analogica A1: ";
      int b = analogRead(A1);
      webpage += b;
      webpage += "</h2>";
/*
      webpage += "<h3>Temperatura Atual: ";
      webpage += tempC;
      webpage += "</h3>";

      webpage += "<h4>Temperatura Maxima: ";
      webpage += tempMax;
      webpage += "</h4>";

      webpage += "<h4>Temperatura Minima: ";
      webpage += tempMin;
      webpage += "</h4>";
*/
      webpage += "<a href='/?H'>Ligar Led</a>";
      webpage += "<br /><a href='/?L'>Desligar Led</a>";
 
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
 
      sendData(cipSend, 1000, DEBUG);
      sendData(webpage, 1000, DEBUG);
 
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";
 
      sendData(closeCommand, 1000, DEBUG);
    }
  }
}
 
String sendData(String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  
  if(response.indexOf("/?H") > 0) {
    digitalWrite(LED, HIGH);
  } 
  
  if(response.indexOf("/?L") > 0) {
    digitalWrite(LED, LOW);
  }
  
  return response;
}
