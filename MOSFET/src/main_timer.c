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


int main(void){

	teensy_clockdivide(0); //set the clock speed

	set(DDRC,6);

	//clock source and prescalers /256 prescaler 
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
	set(TCCR3A, COM3A0);

    for(;;){	
     	int scale = 256; 
     	float frequency = 1;
		ICR3= 16000000.0/scale/frequency; 

		float duty_cycle = 50; 
		OCR3A = (duty_cycle/100.0)*ICR3; 

		set(TCCR3A, COM3A1);
		set(TCCR3A, COM3A0);
    }
    
    return 0; 
}