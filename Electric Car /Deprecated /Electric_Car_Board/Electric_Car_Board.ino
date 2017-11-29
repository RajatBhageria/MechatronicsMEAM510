#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "modlab1"; //Modlab1
const char* pass = "ESAP2017"; // ESAP2017

unsigned long startTime = millis(); 

//for sending 
IPAddress ipSendto(192,168,1,63);
unsigned int udpRemotePort = 2396; 
unsigned int udplocalPort = 2397;
const int UPD_PACKET_SIZE = 5; 
char udpBuffer[UPD_PACKET_SIZE]; 
WiFiUDP udp;

int motor1ADC; 
int motor2ADC; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  
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

  //Motor 1 
  //Set up the pinmode for motor left direction pin 
  pinMode(D0, OUTPUT); 
  //Set up the pinmode for motor right direction pin 
  pinMode(D1, OUTPUT); 
  //set up pinmode for speed of first  motor 
  pinMode(D6, OUTPUT); 
  digitalWrite(D0, LOW); 
  digitalWrite(D1, HIGH); 

  //Motor 2
  //Set up the pinmode for motor left direction pin 
  pinMode(D3, OUTPUT); 
  //Set up the pinmode for motor right direction pin 
  pinMode(D4, OUTPUT); 
  //set up pinmode for speed of second  motor 
  pinMode(D7, OUTPUT); 
  digitalWrite(D3, LOW); 
  digitalWrite(D4, HIGH); 
}

void loop() {
  // put your main code here, to run repeatedly:

  int ADC1; //set to ADC1 value from the UDP
  int ADC2; //set to ADC2 value from the UDP 
  
  String ADCReads = udpRead(); 

  int indexA = ADCReads.indexOf('A');

  int ADCvalue;
  char motorValue; 

  //get the ADC values and which motor it corresponds to 
  if (indexA == -1){
    ADCvalue = ADCReads.substring(0,4).toInt();
    motorValue = (char) ADCReads[4];
      
  } else{
    ADCvalue = ADCReads.substring(0,indexA).toInt();
    motorValue = (char) ADCReads[4];
  }

  //get the values for first and second motor 
  if (motorValue == 'x') {//first motor
    motor2ADC = ADCvalue; 
  } else if (motorValue = 'y'){//second motor 
    motor1ADC = ADCvalue; 
  }

  double multiplier = 2.3; 
  //send the sinals to the motor if both motors have ADCs from controller
  if (motor1ADC > 0 && motor2ADC > 0){
    //first motor 
    int toWriteFirst = (int) motor1ADC*multiplier; 
    analogWrite(D6,toWriteFirst);
    Serial.print("first: ");
    Serial.println(toWriteFirst);
    
    //second motor 
    int toWriteSecond = (int) motor2ADC*multiplier; 
    analogWrite(D7,toWriteSecond);
    Serial.print("second: ");
    Serial.println(toWriteSecond); 
  }

}

/**
 * Send the String toSend to the ipSendTo
 */
void udpSend(String ADC, int motorValue){

  //send the ADC
  for (int i = 0; i < ADC.length(); i++){
    udpBuffer[i] = (char) ADC[i];
  }

  //zeros to add 
  int zeros = UPD_PACKET_SIZE - (ADC).length()-1; 
  for (int i = ADC.length(); i < (zeros+ADC.length()); i++){
    udpBuffer[i] = (char) 'A'; 
  }
  
  //send which motor it is  
  if (motorValue == 1){
      udpBuffer[UPD_PACKET_SIZE-1] = 'x'; 
  } else if (motorValue ==2){
    udpBuffer[UPD_PACKET_SIZE-1] = 'y'; 
  }
  
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
    //Serial.print("Receiving! ");
    //Serial.println(packetFromOther); 
    //Serial.println(".");
  }
  return packetFromOther; 
}

