/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#define F_CPU 16000000
#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include <stdio.h>

float dutyCycleCalc(float duty_cycle);

int main(void)
{		
	set(DDRB, 6);

    teensy_clockdivide(0); //set the clock speed
    //teensy_led(ON);			// turn on the on board LED
    //teensy_wait(1000);		// wait 1000 ms when at 16 MHz 
	//teensy_b6(ON);			// turn on the on board LED

	float duty_cycle = 50.0;
	float frequency = 20.0;
	float period = 1.0 /frequency;
	float wait_time = (period / 2.0)*1000;
		
    for(;;){
        //teensy_led(TOGGLE);	// switch the led state
		toggle(PORTB,6);
        teensy_wait(2.0*wait_time*(duty_cycle/100));		// wait		
		toggle(PORTB,6);
		teensy_wait(2.0*wait_time*(100-duty_cycle)/100);		// wait
    }
    return 0;   /* never reached */

}