#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
 

int main(void)
{
    clear(DDRD,6);              // set D6 as input
 
    // Initialize USB connection
    m_usb_init();

    //////////////////// LED Pin initialize /////////////////////

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

//////////////////////// ADC CODE /////////////////////////////

    // Set voltage reference to Vcc
    clear(ADMUX,REFS1);
    set(ADMUX,REFS0);

    // Set ADC Prescaler to 128 --> clock speed should be 125 kHz
    set(ADCSRA,ADPS2);
    set(ADCSRA,ADPS1);
    set(ADCSRA,ADPS0);
 
    // Disable digital inputs
    set(DIDR2,ADC9D);

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
 

    for(;;){
        int a = ADC;
        teensy_wait(100);
        m_usb_tx_uint(a); //read ADC
        m_usb_tx_string("\n");

//////////////////////////// logic for health as read from comms from ESP
        if (a > 100){
            set(PORTB,0);
        } else{
            clear(PORTB,0);
        }
        if (a > 200){
            set(PORTF,1);
        } else{
            clear(PORTF,1);
        }
       
        if (a > 300){
            set(PORTF,4);
        } else{
            clear(PORTF,4);
        }
       
        if (a > 400){
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
   
 }
 
 