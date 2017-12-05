/* Name: main.c
 * Author: Jeremy Saslaw
 * Description: Lab 1-3, Problem 4
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"

// Declare variables for the phototransistor 
volatile int count = 0;
volatile int frequency = 0;
int a = 0;
int b = 0;

//for the esp health 
int health = 0; 

int main(void){

    // Initialize variables
    int deviceInUse = 1;            // 1 = Potentiometer, 2 = Phototransister

    // Initialize pins for signal from phototransistor
    set(DDRD,3);                // Set output LED 
    clear(DDRF,0);              // Set phototransistor as input

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
    clear(DDRD,6);              // Set ESP as input 

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

    ////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////// ADC Portion //////////////////////////////////

    // Set voltage reference to Vcc
    clear(ADMUX,REFS1);
    set(ADMUX,REFS0);

    // Set ADC Prescaler to 128 --> clock speed should be 125 kHz
    set(ADCSRA,ADPS2);
    set(ADCSRA,ADPS1);
    set(ADCSRA,ADPS0);

    // Disable digital inputs
    set(DIDR2,ADC9D); //ESP
    set(DIDR0,ADC0D); //Photo

    // Set trigger to free running mode
    set(ADCSRA,ADATE);

    // Set Phototransistor (D6 ADC9)
    set(ADCSRB,MUX5);
    clear(ADMUX,MUX2);
    clear(ADMUX,MUX1);
    set(ADMUX,MUX0);

    // Enable ADC subsystem
    set(ADCSRA,ADEN);

    // Begin conversion
    set(ADCSRA,ADSC);

    ////////////////////////////////////////////////////////////////////////////////

    for(;;){ 
        
        // If ADC has registered an input from somewhere
        if(bit_is_set(ADCSRA,ADIF) != 0){

            // ESP
            if(deviceInUse == 1){ // ESP 
                // Change ADC input to ESP
                clear(ADCSRA,ADEN);                 // To avoid spurious behavior
                
                // Set ESP (D6 ADC9)
                set(ADCSRB,MUX5);
                clear(ADMUX,MUX2);
                clear(ADMUX,MUX1);
                set(ADMUX,MUX0);

                set(ADCSRA,ADEN);                   // Re-Enable ADC subsystem
                set(ADCSRA,ADSC);                   // Begin conversion again

                m_usb_tx_string("ESP");
                m_usb_tx_string("\n");

                //get health 
                health = ADC

                // Next time use phototransistor
                deviceInUse = 2;
            }
            
            // Phototransistor
            else{
                // Change ADC input to phototransistor 
                clear(ADCSRA,ADEN);                 // To avoid spurious behavior
                
                //set pins for the phototransistors 
                clear(ADCSRB,MUX5);
                clear(ADMUX,MUX2);
                clear(ADMUX,MUX1);
                clear(ADMUX,MUX0);

                //start conversions 
                set(ADCSRA,ADEN);                   // Re-Enable ADC subsystem
                set(ADCSRA,ADSC);                   // Begin conversion again

                // Determine frequency of light
                m_usb_tx_string("phototransistor");
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
                // Next time use ESP
                deviceInUse = 1;
            }
        }

        //turn on the health lights 
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