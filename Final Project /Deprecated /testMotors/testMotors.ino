void setup() {
  // put your setup code here, to run once:
  //Motor 1 
  //Set up the pinmode for motor left direction pin 
  pinMode(D0, OUTPUT); 
  //Set up the pinmode for motor right direction pin 
  pinMode(D1, OUTPUT); 
  //set up pinmode for speed of first  motor 
  pinMode(D2, OUTPUT); 
  digitalWrite(D0, HIGH); 
  digitalWrite(D1, LOW); 

  //Motor 2
  //Set up the pinmode for motor left direction pin 
  pinMode(D3, OUTPUT); 
  //Set up the pinmode for motor right direction pin 
  pinMode(D4, OUTPUT); 
  //set up pinmode for speed of second  motor 
  pinMode(D5, OUTPUT); 
  digitalWrite(D3, HIGH); 
  digitalWrite(D4, LOW); 
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(D2,1000);
  analogWrite(D5,1000);
}


