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

int find_maximum(int a[], int n);
//int countsStaysSame(int a[], int n,int max);

int main(void)
{		
	m_usb_init();
	char m_usb_isconnected();

	//clock source and prescalers 1024 prescaler 
	set(TCCR3B, CS32);
	clear(TCCR3B, CS31);
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

	//Disable digital inputs for pin B4
	set(DIDR2,ADC11D);

	//triggering for ADC 
	set(ADCSRA,ADATE);

	//Single-Ended Channel Selection pin B4
	set(ADCSRB,MUX5);
	clear(ADMUX,MUX2);
	set(ADMUX,MUX1);
	set(ADMUX,MUX0);

	//Starting the Conversion Process foR ADC 
	set(ADCSRA,ADEN);
	set(ADCSRA,ADSC);

	//Set B4 as an input for the ADC 
    clear(DDRB,4); //clear pin B4 to be an input

    //set C6 to be a timer 
    set(DDRC,6);

    //set the outputs for the LEDs
    set(DDRB, 6); //blue for 1157
	set(DDRB, 5); //red  for 226


    for(;;){	
	    int frequency = 50;
		int scale = 64; 
		ICR3= 16000000.0/scale/frequency;

		int duty_cycle = 50.0;
		OCR3A = (duty_cycle/100.0)*ICR3;  

	    teensy_clockdivide(0); //set the clock speed
	
		//find max value in the last few ADC values 
		int size = 8;
		int arr[8] = {0};
		int i; 
		for(i = 0; i < size;i++){
			arr[i] = ADC; 
		}
	
		int maxValue = find_maximum(arr,size);

		//1 if on 
		//0 if off 
		int blue = 0; 

		//1 if on 
		//0 if off 
		int red = 0;

		//set flags for values of ADC based on max value 
		if (maxValue > 150){
			blue = 1;
			red = 0;
		}
		else if (maxValue > 140){
			blue = 0;
			red = 1;
		} else {
			blue = 0; 
			red = 0;
		}

		//turn on LEDs based on magnitude 
		if (blue==1){
			set(PORTB,6);
			clear(PORTB,5);
		}
		else if (red==1){
			set(PORTB,5);
			clear(PORTB,6);
		} else{
			clear(PORTB,5);
			clear(PORTB,6);
		}
		
/*
		int count = countsStaysSame(ADC,2);

		m_usb_tx_uint(count);
		m_usb_tx_string("\n");
		if (count > 4){
			//red or 226 Hz
			set(PORTB,6);
			clear(PORTB,5);
		} else if (count > 1){
			//blue of 1126 Hz
			set(PORTB,6);
			clear(PORTB,5);
		} else{

		}
*/
    }
    
    return 0;   /* never reached */
}

int find_maximum(int a[], int n) 
{
  int max;
  int c;
  max = a[0];
 	
  for (c = 1; c < n; c++) {
    if (a[c] > max) {
       max = a[c];
    }
  }
 
  return max;
}
/*
int countsStaysSame(int newInt, int max){
  int c;
  int old = a[0];

  for (c = 1; c < n; c++) {
  	int newMin = a[c] - max; 
  	int newMax = a[c] + max;
    if (old > newMin && old < newMax) {
       count++;
       old = a[c];
    } else{
    	break;
    }
  }
  return count; 
}*/