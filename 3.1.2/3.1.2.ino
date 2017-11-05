unsigned long startTime = millis(); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  pinMode(A0, INPUT);     
  pinMode(D0, OUTPUT);
}

void loop() {
  int read = analogRead(A0);

  int period = findPeriod(read); 
  
  if (clock(period)){
      digitalWrite(D0,LOW);
      Serial.println("ON");
  } else{
      digitalWrite(D0,HIGH);
      Serial.println("OFF");
  }
 
  Serial.println(period);
}

/*
 * In Milliseconds 
 */
int findPeriod(int ADC){
  return (int) ADC * 10.18423 -0.921187308;
}

/*
 * Checks whether to turn the clock on or off 
 */
boolean clock(int period){
   long diffTime = millis() - startTime;
   int secondsPastSinceLastTime = diffTime % period;
   Serial.println(secondsPastSinceLastTime);
   return (secondsPastSinceLastTime > period/2);
}
