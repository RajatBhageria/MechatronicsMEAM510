#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//set the ssid and pass of the wifi
const char* ssid = "modlab1"; //Modlab1
const char* pass = "ESAP2017"; // ESAP2017

//for sending 
IPAddress ipSendto(192,168,1,20);
unsigned int udpRemotePort = 2397; 
unsigned int udplocalPort = 2396;
const int UPD_PACKET_SIZE = 6; 
char udpBuffer[UPD_PACKET_SIZE]; 
WiFiUDP udp;

//construct a boolean that will switch every iteration of the loop
//this determines whether we send power to pot 1 or pot 2 for each of the two motors 
boolean firstMotor = false; 

//string value for the team color 
char teamColor = 'r'; 

//melee or not 
boolean melee = false; 

//set up everything and wifi 
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

  //set the pinMode for the team color 
  pinMode(D1, INPUT); 

  //set the pinMode for the melee
  pinMode(D2, INPUT); 
}

//note that we have two potentiometers that independently control the PWM of the two motors 
//this is used for differential driving
//this code allows us to send two ADCs to a single ADC port, A0, on our ESP. 
void loop() {
  int ADC1 = 0; 
  int ADC2 = 0; 
    
  //get the ADC values from the controller.
  //if we want to get the value from the first motor 
  if (firstMotor){
    
    //provide power to the first ADC through inverter (using NAND gates)
    digitalWrite(D0, LOW); 

    //delay to not clog up wifi
    delay(50); 
    
    //read the ADC value 
    ADC1 = analogRead(A0);

  } else{ //we want to get power from the second ADC 
    
    //provide power to the other DDC through inverter
    digitalWrite(D0, HIGH); 
    
    //delay to not clog up wifi
    delay(50); 
    
    //get the reading from second ADC 
    ADC2 = analogRead(A0); 
  }

  //get whether the teamcolor is red or blue. 
  //If red, switch is on 
  //If blue, switch is off 
  int teamColorValue = digitalRead(D1); 
  if (teamColorValue == HIGH) {
    teamColor = 'b';
  } else if (teamColor == LOW){ 
    teamColor = 'r';
  }

  //get whether we want to melee or not 
  int meleeValue = digitalRead(D2);
  if (teamColorValue == HIGH) { // no melee
    melee = false;
  } else if (teamColor == LOW){ //melee
    melee = true;
  }
  
  //actually send the information to the other ESP 
  if (firstMotor){
    //send the ADC value associated with the first motor to the car. 
    //note that we're sending one ADC value at a time during each iteration of the loop.
    //the second parameter tells us that this ADC is associated with the first motor. 
    udpSend(String(ADC1), 1, melee, teamColor);
  }else{
    //send the ADC values associated with the second motor 
    //note that we're sending one ADC value at a time during each iteration of the loop.
    //the second parameter tells us that this ADC is associated with the first motor. 
    udpSend(String(ADC2), 2, melee, teamColor);
  }
  
  //reverse firstMotor so next iteration of the loop, we get the value of the other ADC 
  firstMotor = !firstMotor;
}


/**
 * Send the String toSend to the ipSendTo
 * General Structure: 6 bits 
 * [ADC, ADC, ADC, motorValue, melee, teamColor]
 * [0,   1,   2,   3,          4,     5]
 */
void udpSend(String ADC, int motorValue, boolean melee, char teamColor){
  //send the ADC
  for (int i = 0; i < ADC.length(); i++){
    udpBuffer[i] = (char) ADC[i];
  }

  //Add 'A's to denote if ADC is less than three digits
  int zeros = UPD_PACKET_SIZE - (ADC).length()-3; 
  for (int i = ADC.length(); i < (zeros+ADC.length()); i++){
    udpBuffer[i] = (char) 'A'; 
  }
  
  //send which motor it is  
  if (motorValue == 1){
      udpBuffer[UPD_PACKET_SIZE-3] = 'x'; 
  } else if (motorValue ==2){
    udpBuffer[UPD_PACKET_SIZE-3] = 'y'; 
  }

  //send the melee 
  if (melee){
    udpBuffer[UPD_PACKET_SIZE-2] = 'm'; //melee
  } else{
    udpBuffer[UPD_PACKET_SIZE-2] = 'n'; //no melee
  }
  
  //send the teamColor 
  udpBuffer[UPD_PACKET_SIZE-1] = teamColor; //melee
  
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
