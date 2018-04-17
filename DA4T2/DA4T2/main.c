//Instructions does not say to use a button.
/*Definitions*/
#define F_CPU 8000000UL

/*Libraries*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*Function Definitions*/

/*Global Variables*/
volatile uint8_t ADCvalue;
volatile char step_table[4] = {0b01100110, 0b11001100, 0b10011001, 0b00110011};


int main(void)
{
	DDRB = 0xFF; //port b is an output
	
	//---------------------------------------------------------------
	// INITIALIZE ADC TO READ POTENTIOMETER
	//---------------------------------------------------------------
	ADMUX |= (1 << REFS0) | (1 << ADLAR);	//Vref = AVCC, left adjusted
	ADMUX &= ~(0x0F); //Uses analog input 0
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //prescale clock of 128
	ADCSRA |= (1 << ADEN) | (1 << ADATE); // Enable ADC and auto trigger
	ADCSRA |= (1 << ADSC); // Begin conversion


	//---------------------------------------------------------------
	// CTC Mode
	//---------------------------------------------------------------
	//OCR1A = ADCvalue;
	TCNT1 = 0;
	TCCR1A |= 0;
	TCCR1B |= (1 << WGM12); //CTC Mode, OCR1A as top
	TCCR1B |= (1 << CS12) | (1 << CS10);
	volatile int step;

	while (1)
	{
		while((ADCSRA & (1<<ADIF))==0); // Wait until conversion to finish
		ADCvalue = ADCH; //Value detected from pot
		OCR1A = ADCvalue;
		
		if (TIFR1 & (1 << OCF1A)) //Check if counter & TOP are equal
		{
			
			PORTB = step_table[step];
			step++;
		}
		TIFR1 |= (1 << OCF1A); //TIFR1 holds OCF1A. Clear flag here to reset counter


		step &= 0x03;


	}
}
