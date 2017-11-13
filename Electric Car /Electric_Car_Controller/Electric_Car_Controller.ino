#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "modlab1"; //Modlab1
const char* pass = "ESAP2017"; // ESAP2017

unsigned long startTime = millis(); 

//for sending 
IPAddress ipSendto(192,168,1,20);
unsigned int udpRemotePort = 2397; 
unsigned int udplocalPort = 2396;
const int UPD_PACKET_SIZE = 5; 
char udpBuffer[UPD_PACKET_SIZE]; 
WiFiUDP udp;

boolean firstMotor = false; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  
  //connect to wifi 
  Serial.print(ssid); 
  WiFi.begin(ssid,pass); 
  WiFi.config(IPAddress(192,168,1,63), 
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

  //Set up the pinmode for ADC1 and ADC2 signals 
  pinMode(A0, INPUT); 

  //set the pinMode for the power of the two pots 
  pinMode(D0, OUTPUT); 

}

void loop() {
  //get the ADC values from the controller 
  if (firstMotor){
    //turn on the first ADC 
    digitalWrite(D0, LOW); 
    delay(50); 
    int ADC1 = analogRead(A0);
    //send the ADC values to the car 
    udpSend(String(ADC1), 1);
    firstMotor = !firstMotor;
    
  } else{
    //turn on the second ADC 
    digitalWrite(D0, HIGH); 
    //get the reading from second ADC 
    delay(50); 
    int ADC2 = analogRead(A0);  
    //send the ADC values to the car 
    udpSend(String(ADC2), 2);
    firstMotor = !firstMotor;
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
    Serial.print("Receiving! ");
    Serial.println(packetFromOther); 
    Serial.println(".");
  }
  return packetFromOther; 
}
