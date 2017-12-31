
long Password1 = 8679865; //Please type the card's ID number
long Password2 = 660695; //Please type the card's ID number
int flag = 0, RX_Flag = 0; //serial port sign
char Code[14];
long Num = 0; //decode data
int Door = 5;
int jkl = 0;
long list[] { 4660695, 8679865 };

void Read_ID(void)
{
  int i = 0;
  char temp;
  for (i = 0; (Serial.available() > 0); i++)
  {
    temp = Serial.read();
    Serial.print(temp);
    delay(2);
    Serial.println(i);
    if (temp == 0X02) //recieve the ssrt bit
    {
      flag = 1; i = 0; RX_Flag = 0; //
    }
    if (flag == 1) //detect the start bit and recieve data
    {
      if (temp == 0X03) //detect the end code,
      {
        flag = 0; //zero clearing
        if (i == 13) RX_Flag = 1; //
        else RX_Flag = 0;
        break;
      }
      Code[i] = temp;
    }
  }
  flag = 0; //
}

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.begin(9600);
  Serial.println("This is a test for access control system");
  delay(100);
  Serial.println(Password1);
}
void loop()
{
  int i;
  long temp = 0, time = 0;
  RX_Flag = 0;
  //while (1)
  //{
  Read_ID();
  if (RX_Flag == 1)
  {
    for (i = 5; i < 11; i++) //
    {
      Num <<= 4;
      if (Code[i] > 64)  Num += ((Code[i]) - 55);
      else Num += ((Code[i]) - 48);
      Serial.println(Num);
    }
    Serial.println(Num);

    bool b = false;
    if (Num > 0 && jkl == 0) {
      jkl = 1;
      for (int w = 0; w < sizeof(list) / sizeof(list[0]); w++) {
        if (Num == list[w])
          b = true;
        else
          b = false;
      }
    }
    if (b) {
      trash();
      digitalWrite(13, HIGH);
      delay(5);
      Serial.println("Acesso Liberado!");
      digitalWrite(7, HIGH);
      delay(1000);
      digitalWrite(7, LOW);
      digitalWrite(13, LOW);
      delay(5);
    } else {
      trash();
      Serial.println("Acesso Negado!");
      delay(1000);
    }
    b = false;
    Serial.println(RX_Flag);
    Serial.println(Num);
    jkl = 0;
  }
  //}
}

void trash() {
  RX_Flag = 0; //
  Num = 0; //
}

