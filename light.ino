int isOffOne=0;
int isOffTwo=0;
unsigned int count=0;
int isAlarmOn=0;

void setup() {
  // LDR Sensor One:
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,INPUT);
  

  // LDR Sensor Two:
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,INPUT);

  //In case of using GSM Module
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);

  //Power for LASER
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
  digitalWrite(12,HIGH);
  digitalWrite(10,HIGH);
  digitalWrite(11,LOW);
  digitalWrite(9,LOW);

  //Onboard LED
  pinMode(13,OUTPUT);

  //Initial Powering
  digitalWrite(2,HIGH);
  digitalWrite(3,LOW);
  digitalWrite(5,HIGH);
  digitalWrite(6,LOW);
  

  //Serial.begin(9600);
}

void loop() {
  
  int i=digitalRead(4);
  int j=digitalRead(7);
  
  if(i==1)
  {
    isOffOne=1;
  }
  //Serial.println(isOffOne);
  if(j==1)
  {
    isOffTwo=1;
  }
  //Serial.println(isOffTwo);
  if(isOffOne==1 && isOffTwo==1)
  {
    isAlarmOn=1;
  }

  if(isAlarmOn==1)
  {
    digitalWrite(13,HIGH);
    digitalWrite(0,HIGH);
  }
  
  count++;
  if(count==3000)
  {
    isOffOne=0;
    isOffTwo=0;
    count=0;
  }
  
  

}
