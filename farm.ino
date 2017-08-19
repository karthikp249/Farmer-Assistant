#include <SoftwareSerial.h>
#include <dht.h>

dht DHT;

#define DHT11_PIN 5
 
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

//Global Variables
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String data = "";
String number = "7829965705";
String defaultProtocol = "TCP";
String defaultIP = "54.174.95.66";
String defaultPort = "9000";
String defaultAPN = "bsnlnet";
int irrigating = 0;
int thresholdUp = 450;
int thresholdDown = 250;
int autoirrigate = 1;

int sensorPin1 = A0;
int sensorPin2 = A1;
int sensorPin3 = A2;

int sensorValue1 = 0;
int sensorValue2 = 0;
int sensorValue3 = 0;

//For Security Module
int attempt=0;
int usernotified=0;
 
void setup() {
  //For Connecting to Security Module
  pinMode(11,INPUT);
  
  //For Onboard Light
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);

  //For Relay
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);

  //For LED
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);

  //For Rain
  pinMode(10,INPUT);

  //For Temp/Hum
  pinMode(4,OUTPUT);
  pinMode(6,OUTPUT);
  digitalWrite(4,HIGH);
  digitalWrite(6,LOW);
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication witj Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);

  inputString.reserve(200);
  //data.reserve(300);
  
  debugPrint("Setup Complete!\n");
  //serialSIM800.write("AT+CMEE=1\r"); //Use if you need better Error Reporting

  sendATcommand("AT+CGATT=1",1000);
  sendATcommand("AT+CSTT=\""+defaultAPN+"\"",1500);
  debugSerialPrint(); 
  sendATcommand("AT+CIICR",2000);
  debugSerialPrint();
  sendATcommand("AT+CIFSR",1000);
  debugSerialPrint();  
  
}
 
void loop() {
  delay(5000);
  deletedata();


  //Check Security Module
  int sec = digitalRead(11);
  if(sec == 1)
  {
    attempt++;
    if(attempt == 3 && usernotified == 0)
    {
      attempt = 0;
      sendSecuritySMS(number);
    }
  }
  else
  {
    usernotified = 0;
  }


  //Get DHT11 Reading
  int chk = DHT.read11(DHT11_PIN);
  String Temp = "Temperature:";
  Temp=Temp+DHT.temperature;
  adddata(Temp);
  String Humi="Humidity:";
  Humi=Humi+DHT.humidity;
  adddata(Humi);
  
  // DISPLAY DATA
  debugPrintdouble(DHT.humidity);
  debugPrint(",\t");
  debugPrintdouble(DHT.temperature);
  debugPrint("\n");

  //Get Moisture sensor reading
  sensorValue1 = analogRead(sensorPin1);  
  sensorValue2 = analogRead(sensorPin2);  
  sensorValue3 = analogRead(sensorPin3);

  //Auto ON/OFF
  if(autoirrigate==1)
  {
    int avgMoisture = (sensorValue1+sensorValue2+sensorValue3)/3;
    if(avgMoisture <= thresholdDown)
    {
      //Switch On
      startIrrigation();
    }
    else if(avgMoisture >= thresholdUp)
    {
      //Switch Off
      stopIrrigation();
    }
  }
  
  String Mois1="Moisture1:";
  Mois1=Mois1+sensorValue1;
  adddata(Mois1);
  String Mois2="Moisture2:";
  Mois2=Mois2+sensorValue2;
  adddata(Mois2);
  String Mois3="Moisture3:";
  Mois3=Mois3+sensorValue3;
  adddata(Mois3);

  //Get Rain Sensor
  int Rain=digitalRead(10);
  String RainStr="Rain:";
  if(Rain==1)
  {
    RainStr+="NO";
  }
  else
  {
    RainStr+="YES";
  }
  adddata(RainStr);

  //Get Irrigation
  String IrrigationStr="Irrigating:";
  if(irrigating==1)
  {
    IrrigationStr+="YES";
  }
  else
  {
    IrrigationStr+="NO";
  }
  adddata(IrrigationStr);
  

  //This is just to clear any messages from GSM Module
  clearSerialBuffer();
 
  sendATcommand("AT",500);
  if(serialCheck(0,5,7,"OK"))
  {
    debugPrint("MODULE COMMUNICATING");
    sendATcommand("AT+CMGF=1",1000);  
    if(serialCheck(0,12,14,"OK"))
    {
      debugPrint("SMS FUNCTIONING");
      sendATcommand("AT+CMGR=1",1000);
      if(serialCheck(2,0,7,"CODE ON"))
      {
        debugPrint("SWITCH ON");  
        //Switch On
        startIrrigation();  
        sendATcommand("AT+CMGD=1",500);
        clearSerialBuffer();
      }
      else if(stringCheck(0,8,"CODE OFF"))
      {
        debugPrint("SWITCH OFF"); 
        //Switch Off
        stopIrrigation();            
        sendATcommand("AT+CMGD=1",500);
        clearSerialBuffer();
      }
      else if(stringCheck(0,9,"CODE STAT"))
      {
        debugPrint("SEND STATUS");       
        sendATcommand("AT+CMGD=1",500); 
        clearSerialBuffer();
        
        //SEND Message
        sendSMS(number);
        debugPrint("SMS Sent!");
        clearSerialBuffer();
      }
      else
      {
        sendATcommand("AT+CMGD=1",500);
        clearSerialBuffer();
      }
    }

    clearSerialBuffer();

    //Get updated data
    deletedata();
    adddata(Temp);
    adddata(Humi);
    adddata(Mois1);
    adddata(Mois2);
    adddata(Mois3);
    adddata(RainStr);
    //Get Irrigation
    String IrrigationStr2="Irrigating:";
    if(irrigating==1)
    {
      IrrigationStr2+="YES";
    }
    else
    {
      IrrigationStr2+="NO";
    }
    adddata(IrrigationStr2);

    
    //GPRS Communication
    sendDataOverInternet(defaultProtocol, defaultIP, defaultPort);
    
    clearSerialBuffer();    
  }
  else
  {
    debugPrint("MODULE NOT COMMUNICATING");
  }
  clearSerialBuffer();  

  debugPrint(data);
  //while(1);
  delay(1000);

}

void getserialdata(int mode)
{
  int temp=50;
  while (serialSIM800.available()) {
    // get the new byte:
    char inChar = (char)serialSIM800.read();

    if(mode==4 && inChar!='\n')
    {
      continue; 
    }
    else if(mode==4 && inChar=='\n')
    {
      mode=3;
      continue;
    }
    if(mode==3 && inChar!='\n')
    {
      continue; 
    }
    else if(mode==3 && inChar=='\n')
    {
      mode=2;
      continue;
    }
    if(mode==2 && inChar!='\n')
    {
      continue; 
    }
    else if(mode==2 && inChar=='\n')
    {
      mode=1;
      continue;
    }
    else if(mode==1 && inChar!='\n')
    {
      continue; 
    }
    else if(mode==1 && inChar=='\n')
    {
      mode=0;
      continue;
    }
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\r' || inChar == (char)26) {
      stringComplete = true;
    }
  }
  stringComplete = true;
}


void cleardata()
{
  //Clear the string:
  inputString = "";
  stringComplete = false;
}


void adddata(String str)
{
  data=data+"\n"+str;
  /*data[pos++]='\n';
  for(int i=0; i<str.length(); i++)
  {
    data[pos]=str[i];
    pos++;
  }*/
  
}

void deletedata()
{
  data="";
  /*for( int i = 0; i < sizeof(data);  ++i )
   data[i] = (char)0;
  pos=0;*/
}

//Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
boolean serialCheck(int mode,int first, int last, String text)
{
  getserialdata(mode);
  if(stringComplete) {  
    debugPrint(inputString);
    if(inputString.length()>=last)
    {
      if(inputString.substring(first,last)==text)
      {
        cleardata();
        return true;
      }
    }
  }
  //cleardata();
  return false;  
}

boolean stringCheck(int first, int last, String text)
{
  debugPrint(inputString);
  if(inputString.length()>=last)
  {
    if(inputString.substring(first,last)==text)
    {
      cleardata();
      return true;
    }
  }
  return false;
}

void debugPrint(String debugString)
{
  Serial.print(debugString);
}

void sendATcommand(String ATcommand,int doDelay)
{
  String fullATcommand = ATcommand+"\r\n";
  
  // Used to serially push out a String with Serial.write()
  for (int i = 0; i < fullATcommand.length(); i++)
  {
    serialSIM800.write(fullATcommand[i]);   // Push each char 1 by 1 on each loop pass
  }  
  if(doDelay!=0)
    delay(doDelay);
}

void sendATcommandChar(char ATchar, int doDelay)
{
  serialSIM800.write(ATchar);   // Push each char 1 by 1 on each loop pass
  if(doDelay!=0)
    delay(doDelay);
}

void clearSerialBuffer()
{
  getserialdata(0);
  cleardata();
}

void sendSMS(String num)
{
  sendATcommand("AT+CMGS=\"+91"+num+"\"",1000);
  debugSerialPrint();
  for (int i = 0; i < data.length(); i++)
  {
    sendATcommandChar(data[i],0);   // Push each char 1 by 1 on each loop pass
  }
  sendATcommandChar((char)26 ,1000);
  debugSerialPrint();
}

void sendDataOverInternet(String protocol, String IP, String port)
{
  sendATcommand("AT+CIPSTART=\""+protocol+"\",\""+IP+"\",\""+port+"\"",3000);
  
  //Check if Irrigation is set by server
  if(serialCheck(4, 0, 1, "1"))
  {
    alert(0);
    //Switch On
    startIrrigation();
  }
  else if(serialCheck(4, 0, 1, "0"))
  {
    alert(0);
    //Switch Off
    stopIrrigation();
  }
  else
  {
    alert(1);
  }

  sendATcommand("AT+CIPSEND",1000);                  
  debugSerialPrint();
  for (int i = 0; i < data.length(); i++)
  {
    sendATcommandChar(data[i],0);   // Push each char 1 by 1 on each loop pass
  }
  sendATcommandChar((char)26 ,3000);
  debugSerialPrint();
}

void debugSerialPrint()
{
  getserialdata(0);
  Serial.print(inputString);
  cleardata();
}

void debugPrintdouble(double doubleData)
{
  Serial.print(doubleData,1);
}

void startIrrigation()
{
  irrigating=1;
  digitalWrite(12,HIGH);
  digitalWrite(2,HIGH);
}

void stopIrrigation()
{
  irrigating=0;
  digitalWrite(12,LOW);
  digitalWrite(2,LOW);
  
}

void alert(int status)
{
  if(status==1)
    digitalWrite(13,HIGH);
  else
    digitalWrite(13,LOW);
}

void sendSecuritySMS(String num)
{
  sendATcommand("AT+CMGS=\"+91"+num+"\"",1000);
  debugSerialPrint();
  String securityData = "There is a Security Breach in your farm";
  for (int i = 0; i < securityData.length(); i++)
  {
    sendATcommandChar(securityData[i],0);   // Push each char 1 by 1 on each loop pass
  }
  sendATcommandChar((char)26 ,1000);
  debugSerialPrint();
}


