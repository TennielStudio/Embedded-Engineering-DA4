#define F_CPU 8000000UL

/*Library Functions*/
#include <avr/io.h>
#include <util/delay.h>

/*Global Variables*/
volatile uint8_t ADCvalue;

/*Function Definitions*/
void pulses(volatile uint8_t useconds);

int main(void)
{
	DDRB = 0xFF; //port B as an output 
	
	//---------------------------------------------------------------
	// INITIALIZE ADC TO READ POTENTIOMETER
	//---------------------------------------------------------------
	ADMUX |= (1 << REFS0) | (1 << ADLAR);	//Vref = AVCC, left adjusted
	ADMUX &= ~(0x0F); //Uses analog input 0
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //prescale clock of 128
	ADCSRA |= (1 << ADEN) | (1 << ADATE); // Enable ADC and auto trigger
	ADCSRA |= (1 << ADSC); // Begin conversion

	//---------------------------------------------------------------
	// STEPPER MOTOR INTERFACING
	//---------------------------------------------------------------
	ICR1 = 4999;
	OCR1A = 0; //25% DC 
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1); //OCR1A to compare match on high 
	
	//Fast PWM
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	
	TCCR1B |= (1 << CS11) | (1 << CS10);
	
    while (1) 
    {
		while((ADCSRA & (1<<ADIF))==0); // Wait until conversion to finish
		ADCvalue = ADCH; //Value detected from pot
		
		pulses(ADCvalue);
		
		
    }
}

void pulses(volatile uint8_t useconds)
{
	OCR1A = (useconds);
}
