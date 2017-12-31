#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#define BUTTON 7
#define LED 3

#define RPIN 5
#define GPIN 6
#define BPIN 9

#define DEBUG 1 // Debug output to serial console

int last;

// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 178);
//IPAddress server(172, 16, 0, 2);


EthernetClient ethClient;
PubSubClient client("m12.cloudmqtt.com", 10057, callback, ethClient);

void setup()
{
	pinMode(BUTTON, INPUT_PULLUP);
	pinMode(LED, OUTPUT);
	pinMode(RPIN, OUTPUT);
	pinMode(GPIN, OUTPUT);
	pinMode(BPIN, OUTPUT);

	Ethernet.begin(mac, ip);

#if DEBUG
	Serial.begin(115200);
	Serial.println(Ethernet.localIP());
#endif
	//client.setCallback(callback);
	// Note - the default maximum packet size is 128 bytes. If the
	// combined length of clientId, username and password exceed this,
	// you will need to increase the value of MQTT_MAX_PACKET_SIZE in
	// PubSubClient.h	
}

void loop()
{
	reconnect();
	int statusb = digitalRead(BUTTON);

	if (statusb != last){
		last = statusb;
		client.publish("BUTTON", statusb == 1 ? "ON" : "OFF");
		//client.publish("BUTTON", String(statusb).c_str());
		
#if DEBUG
		Serial.println(statusb);
#endif
	}	
	delay(2000);
}

void reconnect(){
	if (!client.connected()){
#if DEBUG
		Serial.println("Try to connect");
#endif
		if (client.connect("arduinoClient2", "gfrroixd", "82KkGg56AvGV")) {
			client.publish("Connect", "hello world");
			client.subscribe("BUTTON/#");
#if DEBUG
			Serial.println("Connection Successful");
#endif
		}
		else
		{
#if DEBUG
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 2 seconds");
#endif
		}
	}	
	client.loop();
}

void callback(char* topic, byte* p_payload, unsigned int length) {
	String payload;
#if DEBUG
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
#endif
	for (int i = 0; i<length; i++) {
#if DEBUG
		Serial.print((char)p_payload[i]);
#endif
		payload.concat((char)p_payload[i]);
	}
	if (String("BUTTON/cmd").equals(topic)) {
		analogWrite(LED, payload.toInt());
	}
	Serial.println();
}