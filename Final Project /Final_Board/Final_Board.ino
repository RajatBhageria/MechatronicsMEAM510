#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//set up the wifi
const char* ssid = "modlab1"; //Modlab1
const char* pass = "ESAP2017"; // ESAP2017

//set up local and remote ports and packet sizes 
IPAddress ipSendto(192,168,1,63);
unsigned int udpRemotePort = 2396; 
unsigned int udplocalPort = 2397;
const int UPD_PACKET_SIZE = 5; 
char udpBuffer[UPD_PACKET_SIZE]; 
WiFiUDP udp;

//initialize values of the motors 
int motor1ADC; 
int motor2ADC; 

//initialize variable for when received go from 2390
boolean receivedGo = false; 
unsigned int udpGOPort = 2390; 

//initialize the health, cooling period
int health = 100; 
int coolingPeriod = 10; 
int attackDamage = 10; 

//runs once intially to set up wifi and pinModes
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
  //set up pinmode for speed of first  motor 
  pinMode(D3, OUTPUT); 
  //set the direction of the first motor 
  digitalWrite(D0, LOW); 

  //Motor 2
  //Set up the pinmode for motor  direction pin 
  pinMode(D7, OUTPUT); 
  //set up pinmode for speed of second  motor 
  pinMode(D4, OUTPUT); 
  //set the direction for the second motor 
  digitalWrite(D7, LOW); 

  //initialize the pinMode for the servo 
  pinMode(D1, OUTPUT); 

  //initialize the pinMode for the team color light 
  pinMode(D2, OUTPUT); 
}

//runs over and over 
void loop() {
  
  int ADC1; //set to ADC1 value from the UDP
  int ADC2; //set to ADC2 value from the UDP 

  //get the ADC values from the controller 
  String ADCReads = udpRead(); 

  //find the length of the String ADCReads. 
  //Note that this is because ADC values can range from 0 to 4 digits. In our code, 
  //an A shows that the ADC value is less than 4 digits 
  int indexA = ADCReads.indexOf('A');

  //instantiate variables that we'll parse from the ADC reads
  //this is an ADC value from ADCReads
  int ADCvalue;
  //motorValue tells us 'x' for the first motor and 'y' for the second motor. 
  char motorValue; 

  //get the ADC values and which motor it corresponds 
  //in the case that the ADC value is four chars, there will be no 'A' in our ADCReads string 
  if (indexA == -1){
    //We then take the substring and convert it to an int. 
    ADCvalue = ADCReads.substring(0,3).toInt();
    
    //the motorValue ('x' or 'y' is always the fourth char in ADCReads from the controller. 
    motorValue = (char) ADCReads[3];
    
  } else{
    //in the case that the ADC value is less than four chars, there will be an 'A' in the ADCReads
    //And so we find the substring of the string to find the ADC value and convert it to an int. 
    ADCvalue = ADCReads.substring(0,indexA).toInt();
    
    //the motorValue ('x' or 'y' is always the fourth char in ADCReads from the controller. 
    motorValue = (char) ADCReads[3];
  }

  //get the values for first and second motor 
  if (motorValue == 'x') {//first motor
    //set the motor1ADC value to actuate as the ADCValue parsed from ADCReads. 
    motor1ADC = ADCvalue; 
    
  } else if (motorValue = 'y'){//second motor 
    //set the motor2ADC value to actuate as the ADCValue parsed from ADCReads. 
    motor2ADC = ADCvalue; 
  }

  //get whether melee is being implemented 
  boolean melee = ADCReads[4];
  
  //get the team color 
  char teamColor = ADCReads[5];

  //only go if the health is greater than zero. 
  if (health > 0){
    //create a multiplier since our pots on the controller only go from 0 to 400. 
    //since we want the full resolution of 0 to 1000, we multiply all the ADC values by 2.3
    //before actuating. 
    double multiplier = 2.3; 
    
    //send the sinals to the motor if and only if  both motors have ADCs from controller
    if (motor1ADC > 0 && motor2ADC > 0){
      //first motor 
      int toWriteFirst = (int) motor1ADC*multiplier; 
      //write the scaled adc value to the first motor. 
      analogWrite(D3,toWriteFirst);
      //print to test. 
      Serial.print("first: ");
      Serial.println(toWriteFirst);
      
      //second motor 
      int toWriteSecond = (int) motor2ADC*multiplier; 
      //write the scaled adc value to the second motor. 
      analogWrite(D4,toWriteSecond);
      //print to test. 
      Serial.print("second: ");
      Serial.println(toWriteSecond); 
    }

    //display the light of the team color
    //may have to switch high and low depending on which color gets inverter 
    if (teamColor == 'r'){
      digitalWrite(D5, LOW); 
    } else if (teamColor == 'b'){
      digitalWrite(D5, HIGH); 
    }

    //do the meleeing and display that you are meleeing 
    if (melee){
      //make the melee arm go 
      digitalWrite(D1, LOW);
    }
    
      
    //display the health 

    //display whether healing 

    //
    
  }
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

