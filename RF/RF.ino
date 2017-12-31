int x,startbit,ctr,dataok,t1,larpulso,larpulso1,larpulso2,larpulso3,larpulso4,bt1,bt2,antcode=0;    
  unsigned long _data=0; // DATA é o Código recebido do HT6p20 todos os 28 BITS onde, 22 do coodigo+2 dos botões+4 do anti-codigo;    
  unsigned long _dur,_dur1; // Duração do Pulso    
  byte  _pin; // PINO receber o sinal RF digital 2    
  int liga_desliga = 1; //variável para saber se o led/alarme está ligado ou desligado    
  int resultado = 0; //resultado do módulo (%) da divisão, se restar ele é 1, se não restar é 0    
  void setup(){    
  _pin=2;    
  Serial.begin(9600);    
  pinMode(2, INPUT);// pino 2 DATA do receptor Rx 433mhz    
  pinMode(13,OUTPUT); //led    
  }    
  void loop(){    
  digitalWrite(13, digitalRead(2)); //Pisca o LED conforme o valor da entrada digital, testar interferencias.    
  if (startbit==0)    
  {// Testa o tempo piloto até o Bit de inicio;    
  _dur = pulseIn(_pin, LOW);    
  if(_dur > 8000 && _dur < 12000 && startbit==0)    
  {    
   larpulso=_dur/23;    
   larpulso1=larpulso-50;    
   larpulso2=larpulso+50;    
   larpulso3=larpulso+larpulso-50;    
   larpulso4=larpulso+larpulso+50;    
   startbit=1;    
   _dur=0;    
   _data=0;    
   dataok=0;    
   ctr=0;    
  }    
  }    
  // Se o Bit de inicio OK ENTÃO Inicia a medição do tempo em Nivel ALTO dos sinais, e testa se o tempo está na faixa.    
  if (startbit==1 && dataok==0 && ctr < 28)    
  {    
   ++ctr;    
   _dur1 = pulseIn(_pin, HIGH);    
   if(_dur1 > larpulso1 && _dur1 < larpulso2) // Se a largura de pulso é entre 1/4000 e 1/3000 segundos    
   {    
   _data = (_data << 1) + 1; // anexar um * 1 * para a extremidade mais à direita do buffer    
   }    
   else if(_dur1 > larpulso3 && _dur1 < larpulso4) // Se a largura de pulso é entre 2/4000 e 2/3000 segundos    
   {    
   _data = (_data << 1); // anexar um * 0 * para a extremidade mais à direita do buffer    
   }    
   else    
   {    
   /* força finalização do laço */    
   startbit=0;    
   }    
  }    
  if (ctr==28){    
   if (bitRead(_data,0)==1)    
   {    
    if (bitRead(_data,1)==0)    
    {    
    if (bitRead(_data,2)==1)    
     {    
     if (bitRead(_data,3)==0)    
     {    
     antcode=1;    
     }    
     }    
    }    
   }    
   if (antcode==1)    
   {// Se foram recebidos todos os 28 Bits, o valor vai para a variavel _data e pode ser usada como exemplo abaixo.    
    dataok=1;    
    Serial.println(_data,DEC);//Alteração de binário (BIN) para decimal(DEC) para conseguir fazer a comparação    
    //Serial.println(_data,BIN); //Caso queira ver o código em binário e só retirar o comentário dessa linha    
    ctr=0;    
    startbit=0;    
    antcode=0;    
    delay(100);    
    //Comparação dos códigos    
   // Colocar código no botão ligar    
    if (_data==153611413){    
    resultado = liga_desliga % 2;//vê o resultado do módulo (resto da divisão)    
    if (resultado == 0) {//se da divisão não restar nada, ele ligará o led/alarme    
    digitalWrite(13, LOW);    
    }else{//se não, ele desliga    
    digitalWrite(13, HIGH);    
    }    
    liga_desliga++;    
    }    
   }//fim do decodificador    
  }//fim do if (ctr==28)}    
  }//fim do loop    
