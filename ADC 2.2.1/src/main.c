#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"

typedef int bool;
#define true 1
#define false 0
#define check(reg,bit)		(bool)(reg & (1<<(bit)))

void setADCPort(char pPinLetter, int pPinNumber);

int getADCPort();

int ADCPinNumber = 0;

int main(void){		
	teensy_clockdivide(0);
	m_usb_init();

	//ADC Voltage ref as Vcc // no change 
	clear(ADMUX, REFS1);
	set(ADMUX,REFS0);

	//ADC prescaler of 128 // no change 
	set(ADCSRA,ADPS2);
	set(ADCSRA,ADPS1);
	set(ADCSRA,ADPS0);	

	//set the port to test 
	setADCPort('D',7);

	set(ADCSRA,ADEN);
	set(ADCSRA,ADSC);

	for(;;){
		int read = getADCPort();
		m_usb_tx_uint(read);
		m_usb_tx_string("\n");
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
	int a=0;
	if (check(ADCSRA,ADIF)){
		a = ADC;
		set(ADCSRA,ADIF);
	}
	return ADC; 
}
