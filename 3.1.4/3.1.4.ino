#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Mechatronics"; //Modlab1
const char* pass = "YayFunFun"; // ESAP2017

unsigned long startTime = millis(); 

String packetFromOther = "80AA*";

//for sending 
IPAddress ipSendto(192,168,1,16);
unsigned int udpRemotePort = 2397; 
unsigned int udplocalPort = 2396;
const int UPD_PACKET_SIZE = 5; 
char udpBuffer[UPD_PACKET_SIZE]; 
WiFiUDP udp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 

  //Set pinmode for the switch 
  pinMode(D1,INPUT);  

  //set the pinmode for the LED to show changing blinking speeds 
  pinMode(D3, OUTPUT); 

  //Set pinmode for onboard led to show winning 
  pinMode(LED_BUILTIN,OUTPUT); 
  
  //connect to wifi 
  Serial.print(ssid); 
  WiFi.begin(ssid,pass); 
  WiFi.config(IPAddress(192,168,1,20), 
              IPAddress(192,168,1,1),  
              IPAddress(255,255,255,0));
  while (WiFi.status()!=WL_CONNECTED){
    delay(500); 
    Serial.print(".");
  }

  //check if wifi connected 
  Serial.println("WiFi Connected"); 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //start the UDP connection 
  Serial.println("Starting UDP");
  udp.begin(udplocalPort); 
  Serial.print("Local Port:");
  Serial.println(udp.localPort());
}

void loop() {
    //Change the blinking on and off of the LED 
    
    //read in the ADC value of the potentiometer. 
    //find the period relative to this ADC
    int ADC = analogRead(A0); 
    int period = findPeriod(ADC); 
        
    //get the led blinking on and off 
    if (clock(period)){
       digitalWrite(D3,LOW);
    }else{
       digitalWrite(D3,HIGH);
    }

    //get the value of the switch 
    int switchValue = digitalRead(D1);
    
    boolean won = false; 
    
    //get the packet from the other person
    delay(1);
    //get the period and the swtich value of the other person
    String packetFromOther = udpRead();

    int indexA = packetFromOther.indexOf('A');

    int periodOfOtherIP;
    char otherSwitchValue; 
    
    if (indexA == -1){
      periodOfOtherIP = packetFromOther.substring(0,4).toInt();
      otherSwitchValue = (char) packetFromOther[4];
      
    } else{
      periodOfOtherIP = packetFromOther.substring(0,indexA).toInt();
      otherSwitchValue = (char) packetFromOther[4];
    }
    
    //check if other person's switch pressed
    if (otherSwitchValue=='*'){
      if (periodWithinRange(period, periodOfOtherIP)){
        won = false;
      } else{
        won = true; 
      }
    }
    
    //my switch Pressed 
    if (switchValue==LOW){
      
      //Send a packet to the other ESP
      udpSend(String(period),switchValue);   
         
      if (periodWithinRange(period, periodOfOtherIP)){
        won = true;
      } else{
        won = false; 
      }
    }

    //turn on the LED if you won
    if (won){
      digitalWrite(LED_BUILTIN,LOW); 
    } else{
      digitalWrite(LED_BUILTIN,HIGH); 
    }
}

/** 
 *  Convert a binary string to analog between 0 and 100 
 */
int binaryToAnalog(String binary){
  int numerator = binary[0]*2 + binary[1]*1;
  return (int) 1023 * (numerator/3); 
}

/*
 * find the period In Milliseconds 
 */
int findPeriod(int ADC){
  return (int) ADC * 10.18423 - 0.921187308;
}

/*
 * Checks whether to turn the clock on or off 
 */
boolean clock(int period){
   long diffTime = millis() - startTime;
   int secondsPastSinceLastTime = diffTime % period;
   return (secondsPastSinceLastTime > period/2.0);
}

/**
 * Return true if distance between myPeriod and 
 * otherPeriod < 0.1 
 */
boolean periodWithinRange(int myPeriod, int otherPeriod){
  double epsilon = .10;
  double val = (abs(myPeriod - otherPeriod + 0.0001)/myPeriod);
  return val< epsilon;
}

/**
 * Send the String toSend to the ipSendTo
 */
void udpSend(String period, int switchValue){

  //send the period
  for (int i = 0; i < period.length(); i++){
    udpBuffer[i] = (char) period[i];
  }

  //zeros to add 
  int zeros = UPD_PACKET_SIZE - (period).length()-1; 
  for (int i = period.length(); i < (zeros+period.length()); i++){
    udpBuffer[i] = (char) 'A'; 
  }
  
  //send the switch value  
  udpBuffer[UPD_PACKET_SIZE-1] = '*'; //(char) switchValue;
  
  udp.beginPacket(ipSendto,udpRemotePort); 
  udp.write(udpBuffer, sizeof(udpBuffer)); 
  udp.endPacket(); 

  Serial.print("Sending!: ");
  Serial.println(udpBuffer); 
}


/** 
 *  Read the UDP 
 */
String udpRead(){
  String packetFromOther = "";
  int c = udp.parsePacket(); 
  if (c){
    udp.read(udpBuffer, UPD_PACKET_SIZE);
    for (int i = 0; i < UPD_PACKET_SIZE; i++){
      packetFromOther += (char) udpBuffer[i];
    }
    Serial.print("Receiving! ");
    Serial.println(packetFromOther); 
    Serial.println(".");
  }
  return packetFromOther; 
}

