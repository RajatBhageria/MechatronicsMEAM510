#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Mechatronics";
const char* pass = "YayFunFun"; 

WiFiUDP UPDTestServer; 

unsigned int UDPPort = 2390; 
const int packetSize = 6; 

byte packetBuffer[packetSize]; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 

  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT); 
  pinMode(D3,OUTPUT);  
  
  Serial.print("Connecting to: "); 

  Serial.print(ssid); 
  WiFi.begin(ssid,pass); 
  WiFi.config(IPAddress(192,168,1,20), 
              IPAddress(192,168,1,1),  
              IPAddress(255,255,255,0));

  while (WiFi.status()!=WL_CONNECTED){
    delay(500); 
    Serial.print(".");
  }

  Serial.println("WiFi Connected"); 
  UPDTestServer.begin(UDPPort); 

}

void loop() {
   handleUPDServer(); 
   delay(1); 
}

void handleUPDServer(){
  int c = UPDTestServer.parsePacket(); 
  if (c){
    UPDTestServer.read(packetBuffer, packetSize);
    String myData = "";
    for (int i = 0; i < packetSize; i++){
      myData += (char) packetBuffer[i];
    }
    int red = binaryToAnalog(myData.substring(0,1));  
    int green = binaryToAnalog(myData.substring(2,3)); 
    int blue = binaryToAnalog(myData.substring(4,5)); 

    //analogWrite(D1,red); 
    //analogWrite(D2,green); 
    analogWrite(D3,blue); 
    
    Serial.println(myData); 
    Serial.println(".");
  }
}

int binaryToAnalog(String binary){
  int numerator = binary[0]*2 + binary[1]*1;
  return (int) 1023 * (numerator/3); 
}

