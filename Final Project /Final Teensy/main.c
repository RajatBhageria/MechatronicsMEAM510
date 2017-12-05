 

/* Name: main.c
* Author: Jeremy Saslaw
* Description: Final Project Teensy Code 
*/
 

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
 
typedef int bool;
#define true 1
#define false 0

// Declare variables
volatile int count = 0;
volatile int frequency = 0;
int a = 0;
int b = 0;

int phototransistor = 1; 

int main(void){
    // Enable global interrupts
    sei();

    // Initialize pins     
    // For the Phototransistor and LEDs       
    clear(DDRF,0);              // Set phototransistor as input
    set(DDRD,3);                // Set LED as output
    
    //For the health bar 
    set(DDRB,0);
    set(DDRF,1);
    set(DDRF,4);
    set(DDRF,5);
    set(DDRB,6);
    set(DDRB,5);
    set(DDRF,6);
    set(DDRF,7);
    set(DDRB,4);
    set(DDRD,7);
    clear(DDRD,6);              // set D6 as input

    // Initialize USB connection
    m_usb_init();
 
    /////////////////////////////////// Timer ///////////////////////////////////////

    // Timer 3
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

    // Set Phototransistor (F0 ADC10)
    clear(ADCSRB,MUX5);
    clear(ADMUX,MUX2);
    clear(ADMUX,MUX1);
    clear(ADMUX,MUX0);

    // Enable ADC subsystem
    set(ADCSRA,ADEN);

    // Begin conversion
    set(ADCSRA,ADSC);

    ////////////////////////////////////////////////////////////////////////////////
    
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

        if (phototransistor==1){
            OCR3A = 0;
            ICR3 = 0;
        } else {
            //clear ADC 
            clear(ADCSRA,ADEN);

            //Disable digital inputs for D6 for the health bar 
            set(DIDR0,ADC9D);

            //change to the ESP on pin D6 (ADC 9)
            set(ADCSRB,MUX5);
            clear(ADMUX,MUX2);
            clear(ADMUX,MUX1);
            set(ADMUX,MUX0);

            //set ADC
            set(ADCSRA,ADEN);
            set(ADCSRA,ADSC);

            int health = ADC; 

            m_usb_tx_string("ESP");
            m_usb_tx_uint(ADC);
            m_usb_tx_string("\n");

            if (health > 100){
                set(PORTB,0);
            } else{
                clear(PORTB,0);
            }
            if (health > 200){
                set(PORTF,1);
            } else{
                clear(PORTF,1);
            }
           
            if (health > 300){
                set(PORTF,4);
            } else{
                clear(PORTF,4);
            }
           
            if (health > 400){
                set(PORTF,5);
            }
            else{
                clear(PORTF,5);
            }

            clear(PORTF,6);
            clear(PORTF,7);
            clear(PORTB,6);
            clear(PORTB,5);
            clear(PORTB,4);
            set(PORTD,7);
            
            int scale = 64; 
            int frequency = 2000;
            ICR3= 16000000.0/scale/frequency; 

            float duty_cycle = 100*(ADC/1024);
            OCR3A = (duty_cycle/100.0)*ICR3;
        }

        clear(ADCSRA,ADEN);

        // Set Phototransistor (D7 ADC10)
        clear(ADCSRB,MUX5);
        clear(ADMUX,MUX2);
        clear(ADMUX,MUX1);
        clear(ADMUX,MUX0);

        //Starting the Conversion Process
        set(ADCSRA,ADEN);
        set(ADCSRA,ADSC);

        //reverse the variable 
        if (phototransistor==1){
            phototransistor = 0
        } else{
            phototransistor = 1
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