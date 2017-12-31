#include <SPI.h>  
#include <Ethernet.h>  

String readString;
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  
IPAddress ip(192, 168, 1, 177); // IP address
//IPAddress dnsip(8, 8, 8, 8);
//byte gateway[] {192, 168, 1, 1};
//byte subnet[] {255, 255, 255, 0};
 
EthernetServer server(80);
void setup()  
{  
  Serial.begin(9600);
  Serial.print("Start");
  
  //Ethernet.begin(mac, ip, dnsip, gateway, subnet);
  Ethernet.begin(mac, ip);
  server.begin();        
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  EthernetClient client = server.available();
 
    if (client) {
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
 
                //read char by char HTTP request
                if (readString.length() < 100) {
                    //store characters to string
                    readString += c;
                    //Serial.print(c);
                }
 
                //if HTTP request has ended
                if (c == '\n') {
                    Serial.println(readString);
 
                    client.println(F("HTTP/1.1 200 OK")); //send new page
                    client.println(F("Content-Type: text/html"));                    
                    client.println();
                    client.println(F("<html>"));
                    client.println(F("<head>"));
                    client.println("<meta http-equiv='refresh' content='20;URL=/'>");
                    client.println(F("<title>Arduino - CadoreTecnologia</title>"));
                    client.println(F("<link href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.1/css/bootstrap.min.css' rel='stylesheet'></link>"));
                    client.println(F("</head>"));
                    client.println(F("<body>"));
                    client.println(F("<div class='jumbotron'>"));

                    client.println(F("<a class='btn btn-success btn-lg' href='?S1Ligado'>Ligar</buttLigado>"));
                    
                    client.println(F("</div>"));
                    client.println(F("</body>"));
                    client.println(F("</html>"));
                    delay(100);
 
                    //stopping client
                    client.stop();
                    //clearing string for next read
                    readString="";
                }
            }
        }
    }
}














