#include <OneWire.h>
OneWire  leitortag(2);    // 2 e' o pino do Arduino ligado ao leitor 

void setup(void) {
  Serial.begin(9600);
}

byte addr[8];      // buffer global para armazenar enderecos de tags

void loop(void) {
  if(consultaTag()){
      Serial.print("iButton em hexadecimal: ");
      for(byte i=4;i>0;i--) {
           String digitos=String(addr[i], HEX);
           if (digitos.length()==1) digitos="0" + digitos;
           Serial.print(digitos);           
      }
      Serial.println();
      unsigned long tagdecimal=(unsigned long)addr[1];
      tagdecimal=tagdecimal|((unsigned long)addr[2]) << 8;
      tagdecimal=tagdecimal|((unsigned long)addr[3]) << 16;
      tagdecimal=tagdecimal|((unsigned long)addr[4]) << 24;
      Serial.print("iButton em decimal....: ");
      Serial.println(tagdecimal);
      Serial.println();
  }
}

boolean consultaTag() {
  boolean leituraok=false;
  if (leitortag.search(addr)) {
    if (OneWire::crc8(addr, 7) == addr[7]) { // testa a verificacao CRC
      if (addr[0]==1) {  // para tags iButton o primeiro byte sempre e' 1 
          leituraok=true;          
      } 
    }  
  }  
 leitortag.reset();
 return leituraok; 
}
