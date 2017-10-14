/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"

typedef int bool;
#define true 1
#define false 0

#define check(reg,bit)		(bool)(reg & (1<<(bit)))\

int main(void)
{		
	teensy_led(ON);

	m_usb_init();

	//clock source and prescalers /64 prescaler 
	clear(TCCR3B, CS32);
	set(TCCR3B, CS31);
	set(TCCR3B, CS30);
		
	//timer modes
	set(TCCR3B, WGM33);
	set(TCCR3B,WGM32);
	set(TCCR3A, WGM31);
	clear(TCCR3A,WGM30);

	//set timer operating mode 
	set(TCCR3A, COM3A1);
	set(TCCR3A, COM3A0);

	//ADC Voltage ref as Vcc 
	clear(ADMUX, REFS1);
	set(ADMUX,REFS0);

	//ADC prescaler 
	set(ADCSRA,ADPS2);
	set(ADCSRA,ADPS1);
	set(ADCSRA,ADPS0);	

	//Disable digital inputs for potentiometer 
	set(DIDR2,ADC12D);

	//now for the phototransistor 
	set(DIDR2,ADC11D);

	//triggering
	set(ADCSRA,ADATE);

	//for the phototransistor
	set(ADCSRB,MUX5);
	clear(ADMUX,MUX2);
	set(ADMUX,MUX1);
	set(ADMUX,MUX0);

	//Starting the Conversion Process
	set(ADCSRA,ADEN);
	set(ADCSRA,ADSC);


	set(DDRC,6);
	teensy_clockdivide(0); //set the clock speed

    for(;;){	
    	m_usb_tx_string("phototransitor");
		m_usb_tx_uint(ADC);
    	m_usb_tx_string("\n");

    	if (ADC < 600){
    		OCR3A = 0;
    		ICR3 = 0;
    		
    	} else{
    		//clear ADC 
    		clear(ADCSRA,ADEN);

    		//change to the potentiometer
    		set(ADCSRB,MUX5);
			set(ADMUX,MUX2);
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);

			//set ADC
			set(ADCSRA,ADEN);
			set(ADCSRA,ADSC);

			m_usb_tx_string("potentiometer");
			m_usb_tx_uint(ADC);
    		m_usb_tx_string("\n");
    		int scale = 64; 
			int frequency = 2000;
			ICR3= 16000000.0/scale/frequency; 

			float duty_cycle = 100*(ADC/1024);
			OCR3A = (duty_cycle/100.0)*ICR3;
			
    	}
		clear(ADCSRA,ADEN);

	    //for the phototransistor
		set(ADCSRB,MUX5);
		clear(ADMUX,MUX2);
		set(ADMUX,MUX1);
		set(ADMUX,MUX0);

		//Starting the Conversion Process
		set(ADCSRA,ADEN);
		set(ADCSRA,ADSC);
    }
    
    return 0;   /* never reached */
}