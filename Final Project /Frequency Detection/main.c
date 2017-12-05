 

/* Name: main.c
* Author: Jeremy Saslaw
* Description: Lab 1-4, Problem 2
*/
 

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"

// Declare variables
volatile int count = 0;
volatile int frequency = 0;
int a = 0;
int b = 0;

int main(void){
    // Enable global interrupts
    sei();

    // Initialize pins            
    clear(DDRD,7);              // Set phototransistor as input
    set(DDRD,3);                // Set LED as output

    // Initialize USB connection
    m_usb_init();
 

    /////////////////////////////////// Timer 3 ///////////////////////////////////////

    // Set clock divide
    teensy_clockdivide(0);
 
    // Turn on clock source to prescaler of 64
    clear(TCCR3B,CS32);
    set(TCCR3B,CS31);
    set(TCCR3B,CS30);

    // Timer mode - Mode 4 (Normal)
    clear(TCCR3B,WGM33);
    set(TCCR3B,WGM32);
    clear(TCCR3A,WGM31);
    clear(TCCR3A,WGM30);
 
    // Set frequency to 10 Hz (10? might be more i forget lol)
    OCR3A = 25000;
 
    // Set Interrupt portion
    set(TIMSK3,OCIE3A);


    ///////////////////////////////// ADC Portion //////////////////////////////////

    // Set voltage reference to Vcc
    clear(ADMUX,REFS1);
    set(ADMUX,REFS0);

    // Set ADC Prescaler to 128 --> clock speed should be 125 kHz
    set(ADCSRA,ADPS2);
    set(ADCSRA,ADPS1);
    set(ADCSRA,ADPS0);

    // Disable digital inputs for F0 for phototransistor 
    set(DIDR0,ADC0D);

    // Set trigger to free running mode
    set(ADCSRA,ADATE);

    // Set Phototransistor (F0 ADC0)
    clear(ADCSRB,MUX5);
    clear(ADMUX,MUX2);
    clear(ADMUX,MUX1);
    clear(ADMUX,MUX0);
 
    // Enable ADC subsystem
    set(ADCSRA,ADEN);

    // Begin conversion
    set(ADCSRA,ADSC); 


    for(;;){

        m_usb_tx_uint(frequency);
        m_usb_tx_string("\n");

        a = ADC;

        // Wait .05 ms (20 kHz)
        teensy_wait(.05);

        b = ADC;

        // If rising edge
        if ((b-a) > 1){
            // If falling edge
            while((b-a) > -1){
                a = ADC;
                teensy_wait(.05);
                b = ADC;
            }
           count = count + 1;
        }
    }
    return 0;   /* never reached */
}
 

// Interrupt every .1 seconds
ISR(TIMER3_COMPA_vect){
    frequency = 10*count;
 
    if (frequency > 120 && frequency < 320){
         set(PORTD,3);          
    } else{
        clear(PORTD,3);          
    }
   
    count = 0;
}