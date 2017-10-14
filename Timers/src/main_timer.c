/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
int main(void)
{		
	teensy_led(ON);
	OCR3A = 3000; 
	ICR3 = 12000;
	set(DDRC,6);
	
    teensy_clockdivide(0); //set the clock speed
	
	//clock source and prescalers /64 prescaler 
	clear(TCCR3B, CS32);
	set(TCCR3B, CS31);
	set(TCCR3B, CS30);
	
	//timer modes
	set(TCCR3B, WGM33);
	set(TCCR3B,WGM32);
	set(TCCR3A, WGM31);
	clear(TCCR3A,WGM30);
		
    for(;;){		
		set(TCCR3A, COM3A1);
		set(TCCR3A, COM3A0);
    }
    return 0;   /* never reached */

}