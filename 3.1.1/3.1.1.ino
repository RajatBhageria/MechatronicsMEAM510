void setup() {
  pinMode(D1, INPUT);     
  pinMode(D0, OUTPUT); 
  Serial.begin(115200); 
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(D0,digitalRead(D1));
}


