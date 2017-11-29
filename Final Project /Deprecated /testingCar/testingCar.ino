void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A0,INPUT); 
  pinMode(D0,OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D0, HIGH); 
  int ADC = analogRead(A0); 

  Serial.println(ADC); 
}
