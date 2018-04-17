#define F_CPU 8000000UL

/*Library Functions*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*Global Variables*/
volatile uint8_t ADCvalue;

/*Definitions*/
#define BUTTON_PIN PORTD2
#define BUTTONPIN PIND
#define BUTTONDDR DDRD
#define BUTTONPORT PORTD 
#define dcPos PORTB5
#define dcNeg PORTB4

/*Function Prototypes*/
short checkButtonPress();
void Brake();
void Forward();
int main(void)
{
	DDRB = 0xFF; //PortB output for motor
	DDRD = 0x00; //Input for button
	
   //---------------------------------------------------------------
   // INITIALIZE ADC TO READ POTENTIOMETER
   //---------------------------------------------------------------
   ADMUX |= (1 << REFS0) | (1 << ADLAR);	//Vref = AVCC, left adjusted
   ADMUX &= ~(0x0F); //Uses analog input 0
   ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //prescale clock of 128
   ADCSRA |= (1 << ADEN) | (1 << ADATE); // Enable ADC and auto trigger
	ADCSRA |= (1 << ADSC); // Begin conversion

	
	
	//---------------------------------------------------------------
	// BUTTON INTERFACING
	//---------------------------------------------------------------
	BUTTONPORT |= (1 << BUTTON_PIN);	 //Internal pull-up
	EICRA |= (1 << ISC01); //Set INT0 to trigger on falling edge
	EIMSK |= (1 << INT0); //Turns on INT0
	
	
	
	//---------------------------------------------------------------
	// DC MOTOR INTERFACING
	//---------------------------------------------------------------
	OCR1A = 0; // counter 
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1); //clear oc1a on compare match 
	
	//Fast PWM. Update OCR1A at bottom. Set TOV1 flag on TOP
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	
	TCCR1B |= (1 << CS11); // clock div 8
	
	sei(); //Enables interrupts
	while(1)
	{
		while((ADCSRA & (1<<ADIF))==0); // Wait until conversion to finish
		ADCvalue = ADCH; //Value detected from pot
		ICR1 = (ADCvalue * .95); //potentiometer controls speed

	}
	
	return 0;
}

//---------------------------------------------------------------
// DEBOUNCE BUTTON INPUT 
//---------------------------------------------------------------
ISR(INT0_vect)
{
	
	short buttonIsPressed = checkButtonPress();
	if(buttonIsPressed)
	{
		Forward();
	}
	else
	{
		Brake();
	}
	
}

// Debounces button input
short checkButtonPress()
{
	short matchValue = 0; // match counter
	
	while(!(PIND & (1 << BUTTON_PIN)) && // Check if button is still held down
	matchValue < 10) // Count to 10
	{
		matchValue++;
		_delay_ms(5);
	}
	// return true if button has been held for 50ms
	return (matchValue == 10);
}

void Brake()
{
	PORTB |= (1 << dcPos) | (1 <<dcNeg);
}
void Forward()
{
	PORTB |= (1 << dcPos);
	PORTB &= ~(1 << dcNeg);
}