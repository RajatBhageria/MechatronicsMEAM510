#define F_CPU 16000000
#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include <stdio.h>


int main(void)
{
	set(DDRC, 6);
	set(DDRB, 5);
	set(DDRB, 4);

	teensy_clockdivide(0); //set the clock speed
	//teensy_led(ON);			// turn on the on board LED
	//teensy_wait(1000);		// wait 1000 ms when at 16 MHz
	//teensy_b6(ON);			// turn on the on board LED

	float duty_cycle4 = 10.0;
	float duty_cycle5 = 25.0;
	float duty_cycle6 = 50.0;

	float frequency = 40.0;
	float period = 1.0 /frequency;
	float wait_time = (period / 2.0)*1000;
	
	for(;;){
		toggle(PORTC,6);
		teensy_wait(2.0*wait_time*(duty_cycle6/100)); // wait
		toggle(PORTB,5);
		teensy_wait(2.0*wait_time*(duty_cycle5/100));// wait
		toggle(PORTB,4);
		teensy_wait(2.0*wait_time*(duty_cycle4/100)); // wait
		toggle(PORTB,6);
		teensy_wait(2.0*wait_time*(100-duty_cycle6)/100);// wait
		toggle(PORTB,5);
		teensy_wait(2.0*wait_time*(100-duty_cycle5)/100);// wait
		toggle(PORTB,4);
		teensy_wait(2.0*wait_time*(100-duty_cycle4)/100);// wait
	}
	return 0;   /* never reached */

}
