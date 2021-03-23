/*
 * blink_led_timer.c
 *
 * Created: 21.03.2021 15:27:47
 * Author : DAmth
 */ 

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>


volatile uint16_t timer = 0;

/**
 *	Initiate Direction of PortA (input) and PortB (output).
 */
void init()
{
	DDRA = 0x00;	// Port A input (Button)
	DDRB = 0xff;	// Port B output (LED)
	PORTB = 0xff;	// Da LEDs Active Low sind
}

/**
 *	Initiate 8-Bit Timer/Counter0.
 */
void init_timer()
{
	TCCR0 = (1 << WGM01);	// CTC Mode
	TCCR0 |= (1 << CS01) | (1 << CS00); // Prescaler 64
	OCR0 = 125-1;			// Output Compare Register 0A = ((8000000 Hz/64)/1000 ms) = 125
	TIMSK |= (1 << OCIE0);	// Timer Interrupt Mask Register: activate Compare Interrupt
	sei();					// activate Global Interrupts
}

/**
 *	Delay function with Timer. One Step is 1ms.
 */
void set_delay( uint16_t delay )
{
	timer = 0;
	while( timer <= delay );
}

/**
 *	Count PortB up to show binary second counter. 
 *	If Second 60 is reached reset to 1. 
 */
void count_led( uint8_t *temporary )
{
	if( *temporary > 0x30 )	// Reset wenn 60 Sekunden erreicht wurden
		*temporary = 0x01;

	PORTB = ~*temporary;
}

int main( void )
{
	init();
	init_timer();

	uint8_t temporary = 0;

	while ( 1 )
	{	
		temporary++;
		count_led( &temporary );
		set_delay( 1000 );
	}
}


ISR( TIMER0_COMP_vect ) 
{
	timer++;
}

