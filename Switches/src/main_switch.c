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

#define check(reg,bit)		(bool)(reg & (1<<(bit)))


int main(void)
{		
	teensy_led(ON);

	 m_usb_init();
	char m_usb_isconnected();

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

	//Disable digital inputs 
	set(DIDR2,ADC12D);

	//triggering
	set(ADCSRA,ADATE);

	//Single-Ended Channel Selection
	set(ADCSRB,MUX5);
	set(ADMUX,MUX2);
	clear(ADMUX,MUX1);
	clear(ADMUX,MUX0);

	//Starting the Conversion Process
	set(ADCSRA,ADEN);
	set(ADCSRA,ADSC);


    for(;;){	
	    int frequency;

		//for the switch
    	clear(DDRB,4); //clear pin B4 to be an input
    	set(PORTB,4); //set B4 so it's outputting voltage.

    	//set the frequency based on what the switch value is 
     	if (check(PINB,4)){
     		frequency =100; 
     		if(m_usb_rx_available()){
				m_usb_tx_uint(ADC);
				m_usb_tx_string("\n");
			}
			//m_usb_tx_uint(check(ADC,6));
     	} else{
     		frequency = 20000;
     		if(m_usb_rx_available()){
				m_usb_tx_uint(ADC);
				m_usb_tx_string("\n");
			}
     	}

     	int duty_cycle = 50.0;
		OCR3A = (duty_cycle/100.0)*ICR3; 
		int scale = 64; 
		ICR3= 16000000.0/scale/frequency; 
		set(DDRC,6);
	    teensy_clockdivide(0); //set the clock speed
    }
    
    return 0;   /* never reached */
}