#include <SPI.h>
#include <Ethernet.h>

#define LED_BLUE 7
#define LED_RED 6
#define LED_WHITE 3
#define POT A5

String cmd;

boolean validar_usuario(char * linebuf) {

  char usuario_senha[] = "cadore:a1s2 d3f4";
  int t = strlen(usuario_senha);

  int tamanhoEnc = (((t - 1) / 3) + 1) * 4; //tamanho da string codificada
  char out[tamanhoEnc];
  base64_encode(out, usuario_senha, t + 1 );

  int desconta = 0;
  if ((t % 3) == 1) {
    desconta = 2;
  }
  if ((t % 3) == 2) {
    desconta = 1;
  }

  char out2[tamanhoEnc - desconta];

  byte i;
  for (i = 0; i < (tamanhoEnc - desconta); i++) {
    out2[i] = out[i];
  }
  out2[i] = '\0';

  return ( strstr(linebuf, out2) > 0 );
}

EthernetServer * server;

void iniciar_ethernet() {
  byte ip[4]      = { 192, 168, 1, 177 };
  byte gateway[4] = { 192, 168, 1, 1 };
  byte subnet[4]  = { 255, 255, 255, 0 };
  byte mac[6]     = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  int  porta      = 2659;

  server = new EthernetServer(porta);

  Ethernet.begin(mac, ip, gateway, subnet);         //caso necessario gateway utilizar essa linha
  //Ethernet.begin(mac, ip);
  server->begin();
}

void html_logoff(EthernetClient &client) {
  client.print(F(
                 "HTTP/1.1 401 Authorization Required\n"
                 "Content-Type: text/html\n"
                 "Connnection: close\n\n"
                 "<!DOCTYPE HTML>\n"
                 "<html><head><title>Logoff</title>\n"
                 "<script>document.execCommand('ClearAuthenticationCache', 'false');</script>"  //IE logoff
                 "<script>try {"                                                                //mozila logoff
                 "   var agt=navigator.userAgent.toLowerCase();"
                 "   if (agt.indexOf(\"msie\") != -1) { document.execCommand(\"ClearAuthenticationCache\"); }"
                 "   else {"
                 "     var xmlhttp = createXMLObject();"
                 "      xmlhttp.open(\"GET\",\"URL\",true,\"logout\",\"logout\");"
                 "     xmlhttp.send(\"\");"
                 "     xmlhttp.abort();"
                 "   }"
                 " } catch(e) {"
                 "   alert(\"erro ao fazer logoff\");"
                 " }"
                 "function createXMLObject() {"
                 "  try {if (window.XMLHttpRequest) {xmlhttp = new XMLHttpRequest();} else if (window.ActiveXObject) {xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");}} catch (e) {xmlhttp=false;}"
                 "  return xmlhttp;"
                 "}</script>"
                 "<meta http-equiv='refresh' content=2;url='/'>"
                 "</head><body><h1>Voce n&atilde;o est&aacute; mais conectado.</h1></body></html>\n"));
}
void html_autenticar(EthernetClient &client) {
  client.print(F("HTTP/1.1 401 Authorization Required\n"
                 "WWW-Authenticate: Basic realm=\"Area Restrita\"\n"
                 "Content-Type: text/html\n"
                 "Connnection: close\n\n"
                 "<!DOCTYPE HTML>\n"
                 "<html><head><title>Error</title>\n"
                 "</head><body><h1>401: Acesso n&atilde;o autorizado</h1></body></html>\n"));
}

void html_autenticado(EthernetClient &client, char c) {

  client.println(F("HTTP/1.1 200 OK\n"
                   "Content-Type: text/html\n"
                   "Connection: keep-alive\n\n"
                   "<!DOCTYPE HTML>"
                   "<html>"));
  client.println("<meta http-equiv='refresh' content='20;URL=/'>");
  client.println(F("<title>Arduino - CadoreTecnologia</title>"));
  client.println(F("<link href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.1/css/bootstrap.min.css' rel='stylesheet'></link>"));
  client.println(F("</head>"));
  client.println(F("<body>"));
  client.println(F("<div class='jumbotron'>"));
  client.println(F("<h2>Interface de comando Arduino - CadoreTecnologia</h2>"));

  body(client);

  client.println(F("</body>"));
  //client.println(F("<br><a href='/logoff'>Logoff</a></html>"));
}



void exec_ethernet() {
  EthernetClient client = server->available();
  if (client) {
    char linebuf[80];
    memset(linebuf, 0, sizeof(linebuf));

    int     charCount          = 0;
    boolean autenticado        = false;
    boolean currentLineIsBlank = true;
    boolean logoff             = false;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        linebuf[charCount] = c;
        if (charCount < sizeof(linebuf) - 1) {
          charCount++;
        }
        //Serial.write(c);

        if (c == '\n' && currentLineIsBlank) {
          if (autenticado && !logoff ) {
            html_autenticado(client, c);
          } else {
            logoff ? html_logoff(client) : html_autenticar(client);
          }
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;

          if (strstr(linebuf, "GET /logoff"         ) > 0 ) {
            logoff = true;
          }
          else if (strstr(linebuf, "Authorization: Basic") > 0 ) {
            if ( validar_usuario(linebuf) ) {
              autenticado = true;
            }
          }

          else if (strstr(linebuf, "GET /?bon") > 0 ) {
            cmd = "bon";
          }
          else if (strstr(linebuf, "GET /?boff") > 0 ) {
            cmd = "boff";
          }
          else if (strstr(linebuf, "GET /?ron") > 0 ) {
            cmd = "ron";
          }
          else if (strstr(linebuf, "GET /?roff") > 0 ) {
            cmd = "roff";
          }
          else if (strstr(linebuf, "GET /?won") > 0 ) {
            cmd = "won";
          }
          else if (strstr(linebuf, "GET /?woff") > 0 ) {
            cmd = "woff";
          }

          memset(linebuf, 0, sizeof(linebuf));
          charCount = 0;
        } else if (c != '\r') {
          currentLineIsBlank = false;    // you've gotten a character on the current line
        }
      }
    }

    delay(1);           // give the web browser time to receive the data
    client.stop();      // close the connection:
  }
}

void a3_to_a4(unsigned char * a4, unsigned char * a3);
void a4_to_a3(unsigned char * a3, unsigned char * a4);
unsigned char b64_lookup(char c);

int base64_encode(char *output, char *input, int inputLen) {
  const char b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int i = 0, j = 0, encLen = 0;
  unsigned char a3[3], a4[4];

  while (inputLen--) {
    a3[i++] = *(input++);
    if (i == 3) {
      a3_to_a4(a4, a3);
      for (i = 0; i < 4; i++) {
        output[encLen++] = b64_alphabet[a4[i]];
      }
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++)     {
      a3[j] = '\0';
    }
    a3_to_a4(a4, a3);
    for (j = 0; j < i + 1; j++) {
      output[encLen++] = b64_alphabet[a4[j]];
    }
    while ((i++ < 3))           {
      output[encLen++] = '=';
    }
  }
  output[encLen] = '\0';
  return encLen;
}

void a3_to_a4(unsigned char * a4, unsigned char * a3) {
  a4[0] = (a3[0]  & 0xfc) >> 2;
  a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
  a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
  a4[3] = (a3[2] & 0x3f);
}

unsigned char b64_lookup(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 71;
  if (c >= '0' && c <= '9') return c + 4;
  if (c == '+') return 62;
  if (c == '/') return 63;
  return -1;
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  pinMode(POT, INPUT);

  pinMode(A4, OUTPUT);

  iniciar_ethernet();
}

void loop() {
  exec_ethernet();
  delay(10);
}

void body(EthernetClient &client) {
  if (cmd != "")
  {
    if (cmd == "bon")
    {
      digitalWrite(LED_BLUE, HIGH);
    } else if (cmd == "boff")
    {
      digitalWrite(LED_BLUE, LOW);
    }

    if (cmd == "ron")
    {
      digitalWrite(LED_RED, HIGH);
    } else if (cmd == "roff")
    {
      digitalWrite(LED_RED, LOW);
    }

    if (cmd == "won")
    {
      digitalWrite(LED_WHITE, HIGH);
    } else if (cmd == "woff")
    {
      digitalWrite(LED_WHITE, LOW);
    }
  }

  int S1 = digitalRead(LED_BLUE);
  int S2 = digitalRead(LED_RED);
  int S3 = digitalRead(LED_WHITE);

  client.println(F("<div class='row'>"));
  client.println(F("<div class='col-md-10'>"));
  client.println(F("<table class='table table-bordered'>"));
  client.println(F("<tbody>"));

  //S 1
  client.println(F("<tr><td width=130px><span style='color:#0000FF'>LED AZUL</span> - "));
  if (S1 == HIGH)
  {
    client.println(F("Ligado"));
    client.println(F("</td><td>"));
    client.println(F("<a class='btn btn-danger btn-lg' href='/?boff'>Desligar</buttLigado>"));
  }
  else
  {
    client.println(F("Desligado"));
    client.println(F("</td><td>"));
    client.println(F("<a class='btn btn-success btn-lg' href='/?bon'>Ligar</buttLigado>"));
  }
  client.println(F("</td></tr>"));

  //S 2
  client.println(F("<tr><td width=130px><span style='color:#FF0000'>LED VERMELHO</span> - "));
  if (S2 == HIGH)
  {
    client.println(F("Ligado"));
    client.println(F("</td><td>"));
    client.println(F("<a class='btn btn-danger btn-lg' href='/?roff'>Desligar</buttLigado>"));
  }
  else
  {
    client.println(F("Desligado"));
    client.println(F("</td><td>"));
    client.println(F("<a class='btn btn-success btn-lg' href='/?ron'>Ligar</buttLigado>"));
  }

  //S 3
  client.println(F("<tr><td width=130px><span style='color:#FFFFFF'>LED BRANCO</span> - "));
  if (S3 == HIGH)
  {
    client.println(F("Ligado"));
    client.println(F("</td><td>"));
    client.println(F("<a class='btn btn-danger btn-lg' href='/?woff'>Desligar</buttLigado>"));
  }
  else
  {
    client.println(F("Desligado"));
    client.println(F("</td><td>"));
    client.println(F("<a class='btn btn-success btn-lg' href='/?won'>Ligar</buttLigado>"));
  }

  client.println(F("<tr><td width=130px><span style='color:#000000'>ENTRADA</span>"));
  client.println(F("</td><td>"));
  client.println(F("<span style='color:#000000'>"));
  client.println(analogRead(POT));
  client.println(F("</span>"));

  client.println(F("</td></tr>"));
  cmd = "";
}

