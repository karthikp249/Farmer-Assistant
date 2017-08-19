#include <SoftwareSerial.h>

//ESP TX is connected to Arduino D8
#define ESP_TX_PIN 8
 
//ESP RX is connected to Arduino D7
#define ESP_RX_PIN 7

//Create software serial object to communicate with SIM800
SoftwareSerial serialESP(ESP_TX_PIN,ESP_RX_PIN);

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int attempt=0;

void setup() {
  // On board LED
  pinMode(13,OUTPUT);

  //If GSM Function is needed
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  //Serial.begin(9600);
  //while(!Serial);

  inputString.reserve(200);
  //Serial.println("Setup Complete!");
  
  //Being serial communication witj Arduino and SIM800
  serialESP.begin(115200);
  delay(1000);

  
}

void loop() {
  // put your main code here, to run repeatedly:

  //This is just to clear any messages from GSM Module
  getserialdata(0);
  cleardata();

  serialESP.write("AT\r\n");
  delay(500);
  //Read ESP output (if available) and print it in Arduino IDE Serial Monitor
  getserialdata(0);
  if (stringComplete) {
    //Serial.print(inputString);
    if(inputString.length()>=9 && inputString.substring(7,9)=="OK")
    {
      //Serial.println("MODULE COMMUNICATING");
      cleardata();
      //delay(1000);
      serialESP.write("AT+CWLAP=\"cow1\"\r\n");
      delay(3000);
      getserialdata(2);
      if (stringComplete) {
        //Serial.print(inputString.substring(2,4));
        if(inputString.length()>=4 && inputString.substring(2,4)=="OK")
        {
          //No Alarm 
          digitalWrite(13,LOW);
          attempt=0;
        }
        else
        {
          //Increase attempt
          attempt++;
          
          //Alarm
          if(attempt==3)
          {
            digitalWrite(13,HIGH);
            digitalWrite(0,HIGH);
          }
        }
      }
    }
  }

  //while(1);
  //delay(3000);

}


void getserialdata(int mode)
{
  int temp=50;
  while (serialESP.available()) {
    // get the new byte:
    char inChar = (char)serialESP.read();

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
  // clear the string:
    inputString = "";
    stringComplete = false;
}
