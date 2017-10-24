#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"

typedef int bool;
#define true 1
#define false 0
#define check(reg,bit)		(bool)(reg & (1<<(bit)))

#define inputPot1Letter 'D'
#define inputPot1Number 7  
#define inputPot2Letter 'D'
#define inputPot2Number 6  

#define outputPot1Letter 'B'
#define outputPot1Number 5  
#define outputPot2Letter 'F'
#define outputPot2Number 0 

#define motor1APin 2 /* d*/
#define motor2APin 1 /* d*/
#define speedPin1 2 /* d*/

#define motor3APin 6 /* f*/
#define motor4APin 4 /* f*/
#define speedPin2 7 /* f*/

void setADCPort(char pPinLetter, int pPinNumber);

int getADCPort();

int ADCPinNumber = 0;

int inputPotA = 0; 
int inputPotB = 0; 
int outputPot1 = 0; 
int outputPot2 = 0; 

int joint1Diff = 0; 
int joint2Diff = 0; 

int main(void){		
	//clockdivide of zero!
	teensy_clockdivide(0);
	m_usb_init();
	//set the DDRC for the first motor 
	set(DDRD,motor1APin);
	set(DDRD,motor2APin);
	set(DDRD,speedPin1);
	//set the direction of the motor

	//second motor 
	set(DDRF,motor3APin);
	set(DDRF,motor4APin);
	set(DDRF,speedPin2);
	//set the direction of the motor
	//clear(PORTF,motor3APin);
	//set(PORTF,motor4APin);

	//initialize all the timer stuff 
	//clock source and prescalers 256 prescaler for timer 
	set(TCCR3B, CS32);
	clear(TCCR3B, CS31);
	clear(TCCR3B, CS30);
	//timer modes
	set(TCCR3B, WGM33);
	set(TCCR3B,WGM32);
	set(TCCR3A, WGM31);
	clear(TCCR3A,WGM30);
	//set timer operating mode
	set(TCCR3A, COM3A1);
	clear(TCCR3A, COM3A0);

	//All the ADC stuff 
	//ADC Voltage ref as Vcc // no change 
	clear(ADMUX, REFS1);
	set(ADMUX,REFS0);

	//ADC prescaler of 128 // no change 
	set(ADCSRA,ADPS2);
	set(ADCSRA,ADPS1);
	set(ADCSRA,ADPS0);	

	setADCPort(inputPot1Letter,inputPot1Number);
	setADCPort(inputPot2Letter,inputPot2Number);
	setADCPort(outputPot1Letter,outputPot1Number);
	setADCPort(outputPot2Letter,outputPot2Number);

	int scale = 256;
	float frequency = 1000;
	ICR3= 16000000.0/scale/frequency;

	for(;;){
		//set the ADC port for Pot1 Input  
		//setADCPort(inputPot1Letter,inputPot1Number);
		setClearSingleEndedChannelSelection("1010");
		set(ADCSRA,ADEN);
		set(ADCSRA,ADSC);
		inputPotA = ADC;//getADCPort();
		//display the results for Pot1
		teensy_wait(200);
/*		m_usb_tx_string("Input Pot 1: ");
		m_usb_tx_int(inputPotA);
		m_usb_tx_string("\n");*/

		//clear ADC 
    	clear(ADCSRA,ADEN);
		//set the ADC port for Pot2 Input  
		setClearSingleEndedChannelSelection("1001");
		//setADCPort(inputPot2Letter,inputPot2Number);
		//restart the ADC 
		set(ADCSRA,ADEN);
		set(ADCSRA,ADSC);
		//get the diff betw new val and old val
		inputPotB = ADC;//getADCPort();
		//display the results 
		teensy_wait(200);
/*		m_usb_tx_string("Input Pot 2: ");
		m_usb_tx_int(inputPotB);
		m_usb_tx_string("\n");*/

		float duty_cycle = 0; 

		//clear ADC 
    	clear(ADCSRA,ADEN);
		//set the ADC port for Pot1 Output  
		//setADCPort(outputPot1Letter,outputPot1Number);
		setClearSingleEndedChannelSelection("1100");
		//restart the ADC 
		set(ADCSRA,ADEN);
		set(ADCSRA,ADSC);
		//display the results 
		outputPot1 = ADC;//getADCPort();
		teensy_wait(200);
/*		m_usb_tx_string("Output Pot 1: ");
		m_usb_tx_uint(output1Diff);
		m_usb_tx_string("\n");*/


		//clear ADC 
    	clear(ADCSRA,ADEN);
		//set the ADC port for Pot1 Output  
		//setADCPort(outputPot2Letter,outputPot2Number);
		setClearSingleEndedChannelSelection("0000");
		//restart the ADC 
		set(ADCSRA,ADEN);
		set(ADCSRA,ADSC);
		//display the results 
		outputPot2 = ADC; //getADCPort();
		teensy_wait(200);
/*		m_usb_tx_string("Output Pot 2: ");
		m_usb_tx_uint(output2Diff);
		m_usb_tx_string("\n");*/

		joint1Diff = inputPotA - outputPot1;
		joint2Diff = inputPotB - outputPot2;

		m_usb_tx_string("joint1Diff: ");
		m_usb_tx_int(joint1Diff);
		m_usb_tx_string("\n");

		m_usb_tx_string("joint2Diff: ");
		m_usb_tx_int(joint2Diff);
		m_usb_tx_string("\n");

		// if (joint1Diff > 50){
		// 	clear(PORTD,motor1APin);
		// 	set(PORTD,motor2APin);
		// 	duty_cycle = 100*(joint1Diff/1024);
		// 	OCR3A = (duty_cycle/100.0)*ICR3;
		// } else if (joint1Diff < 50){
		// 	set(PORTD,motor1APin);
		// 	clear(PORTD,motor2APin);
		// 	duty_cycle = 100*(joint1Diff/1024);
		// 	OCR3A = (duty_cycle/100.0)*ICR3;
		// }

		// if (joint2Diff > 50){
		// 	clear(PORTF,motor3APin);
		// 	set(PORTF,motor4APin);
		// 	duty_cycle = 100*(joint2Diff/1024);
		// 	OCR3A = (duty_cycle/100.0)*ICR3;
		// } else if (joint2Diff < 50){
		// 	set(PORTF,motor3APin);
		// 	clear(PORTF,motor4APin);
		// 	duty_cycle = 100*(joint2Diff/1024);
		// 	OCR3A = (duty_cycle/100.0)*ICR3;
		// }
	}

	return 0;
}

void setADCPort(char pPinLetter, int pPinNumber){
	 char pinLetter = pPinLetter; 
	 int pinNumber = pPinNumber;
	 switch(pinLetter){
	 	case 'F': 
	 		switch(pinNumber){
	 			case 0:
	 				ADCPinNumber = 0; 
	 				clear(DDRF,0);
	 				set(DIDR0,ADC0D);
	 				set(ADCSRA,ADATE);
	 				setClearSingleEndedChannelSelection("0000");
	 				break;
	 			case 1:
	 				ADCPinNumber = 1; 
	 				clear(DDRF,1);
	 				set(DIDR0,ADC1D); 	
	 				set(ADCSRA,ADATE);			
	 				setClearSingleEndedChannelSelection("0001");
	 			 	break;
	 			 case 4:
	 				ADCPinNumber = 4; 	 
	 				clear(DDRF,4);	
	 				set(DIDR0,ADC4D); 	
	 				set(ADCSRA,ADATE);				 				
	 				setClearSingleEndedChannelSelection("0100");
	 			 	break;
				case 5:
	 				ADCPinNumber = 5; 
	 				clear(DDRF,5);
	 				set(DIDR0,ADC5D);
	 				set(ADCSRA,ADATE);
	 				setClearSingleEndedChannelSelection("0101");
	 				break;
	 			case 6:
					clear(DDRF,6);
	 				ADCPinNumber = 6; 
	 				set(DIDR0,ADC6D); 	
	 				set(ADCSRA,ADATE);			
	 				setClearSingleEndedChannelSelection("0110");
	 			 	break;
	 			 case 7:
	 				ADCPinNumber = 7; 	 	
	 				clear(DDRF,7);
	 				set(DIDR0,ADC7D); 	
	 				set(ADCSRA,ADATE);		
	 				setClearSingleEndedChannelSelection("0111");
	 			 	break;
	 		}
	 		break;
	 	case 'D':
	 		switch(pinNumber){
	 			case 4:
	 				ADCPinNumber = 8; 
	 				clear(DDRD,4);
	 				set(DIDR2,ADC8D);
	 				set(ADCSRA,ADATE);
	 				setClearSingleEndedChannelSelection("1000");
	 				break;
	 			case 6:
	 				ADCPinNumber = 9; 
	 				clear(DDRD,6);
	 				set(DIDR2,ADC9D); 	
	 				set(ADCSRA,ADATE);			
	 				setClearSingleEndedChannelSelection("1001");
	 			 	break;
	 			 case 7:
	 				ADCPinNumber = 10; 	
	 				clear(DDRD,7); 	
	 				set(DIDR2,ADC10D); 		
	 				set(ADCSRA,ADATE);			 				
	 				setClearSingleEndedChannelSelection("1010");
	 			 	break;
	 		}
	 		break;
	 	case 'B':
	 		switch(pinNumber){
	 			case 4:
	 				ADCPinNumber = 11; 	 
	 				clear(DDRB,4);			
					set(DIDR2,ADC11D); 		
					set(ADCSRA,ADATE);		
	 				setClearSingleEndedChannelSelection("1011");
	 				break;
	 			case 5:
	 				ADCPinNumber = 12; 
	 				clear(DDRB,5);	 					 
					set(DIDR2,ADC12D);
					set(ADCSRA,ADATE);					
	 				setClearSingleEndedChannelSelection("1100");
	 			 	break;
	 			 case 6:
	 				ADCPinNumber = 13; 	 
	 				clear(DDRB,6);
					set(DIDR2,ADC13D); 	
					set(ADCSRA,ADATE);						 
	 				setClearSingleEndedChannelSelection("1101");
	 			 	break;
	 		}
	 		break;
	 }
}


void setClearSingleEndedChannelSelection(char values[]){
	char first = values[0];
	char second = values[1];
	char third = values[2];
	char fourth = values[3];
	if (first=='1'){
		set(ADCSRB,MUX5);
	} else {
		clear(ADCSRB,MUX5);
	}
	if (second=='1'){
		set(ADMUX,MUX2);
	} else {
		clear(ADMUX,MUX2);
	}
	if (third=='1'){
		set(ADMUX,MUX1);
	} else {
		clear(ADMUX,MUX1);
	}
	if (fourth =='1'){
		set(ADMUX,MUX0);
	} else{
		clear(ADMUX,MUX0);
	}
}

int getADCPort(){
	if (check(ADCSRA,ADIF)){
		set(ADCSRA,ADIF);
	}
	return ADC; 
}
