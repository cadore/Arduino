int on = 0; // nao usada, gambiarra desativada

byte MON_PIN = 8; // variavel para porta 8, porta do monitor
byte BAT_PIN = A1; // variavel para porta A1, porta do carregador
byte AUX_PIN = A2; // variavel para porta A2, porta auxiliar

void setup()
{
  Serial.begin(9600); // inicializa o serial a 9600 baud rate
  
  pinMode(BAT_PIN, OUTPUT); //define a porta do carregador como saida
  pinMode(MON_PIN, OUTPUT); //define a porta do monitor como saida
  pinMode(AUX_PIN, OUTPUT); //define a porta auxiliar como saida

  digitalWrite(BAT_PIN, LOW); // }}
  digitalWrite(MON_PIN, LOW); // }}} inicializa ja com todas as portas desligadas
  digitalWrite(AUX_PIN, LOW); // }}
}

void loop() 
{
  /*
   * gambiarra não utilizada
  if(on == 0)
  {
    on = 1;
    //digitalWrite(MON_PIN, HIGH);  
  }
  */
  
  if(Serial.available()) // se houver algum dado no serial
  {    
    int c = Serial.read(); // atribui o valor lido no serial em Int (todo valor do serial retorna um Int)
    switch(c) // Inicia o switch
    {
        case 'B': // caso serial tenha recebido 'B' liga o carregador
        digitalWrite(BAT_PIN, HIGH); 
        break;

        case 'b': // caso serial tenha recebido 'b' desliga o carregador
        digitalWrite(BAT_PIN, LOW);
        break;

        case 'M': // caso serial tenha recebido 'M' liga o monitor
        digitalWrite(MON_PIN, HIGH);
        break;

        case 'm': // caso serial tenha recebido 'm' desliga o monitor
        digitalWrite(MON_PIN, LOW);
        break;

        case 'A': // caso serial tenha recebido 'A' liga a auxiliar
        digitalWrite(AUX_PIN, HIGH);
        break;

        case 'a': // caso serial tenha recebido 'a' liga a auxiliar
        digitalWrite(AUX_PIN, LOW);
        break;
    }    
    Serial.println(c); // retorna o valor da Serial
    delay(200); // delay para garantir que a serial nao receba dados consecutivos e trave
  }
}
