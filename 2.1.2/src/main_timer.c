/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"

#define motor1APin 2
#define motor2APin 3
#define speedPin 6

#define check(reg,bit)		(bool)(reg & (1<<(bit)))

int main(void){

	teensy_clockdivide(0); //set the clock speed

	teensy_led(ON);
	set(DDRD,motor1APin);
	set(DDRD,motor2APin);
	set(DDRC,speedPin);

	clear(PORTD,motor1APin);
    set(PORTD,motor2APin);

    set(PORTC,speedPin);

	float duty_cycle = 50.0;
	float frequency = 0.5;
	float period = 1.0 /frequency;
	float wait_time = (period / 2.0)*1000;
		
    for(;;){
        teensy_wait(2.0*wait_time*(duty_cycle/100));		// wait		
		toggle(PORTD,motor1APin);
    	toggle(PORTD,motor2APin);
    	teensy_wait(2.0*wait_time*(100-duty_cycle)/100);		// wait
    	clear(PORTD,motor1APin);
    	clear(PORTD,motor2APin);
    	teensy_wait(2.0*wait_time*(duty_cycle/100));		// wait		
    	clear(PORTD,motor1APin);
    	set(PORTD,motor2APin);
    }
    return 0;   /* never reached */
}

/*
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
*/