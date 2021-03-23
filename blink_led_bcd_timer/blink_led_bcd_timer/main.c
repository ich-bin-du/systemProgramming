/*
 * blink_led_bcd_timer.c
 *
 * Created: 23.03.2021 12:41:47
 * Author : Dominik Amthor
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
	OCR0 = 125-1;			// Output Compare Register 0 = ((8000000 Hz/64)/1000 ms) = 125
	TIMSK |= (1 << OCIE0);	// Timer Interrupt Mask Register: activate Compare Interrupt
	sei();					// activate Global Interrupts
}

/**
 *	Counts up the LEDs in the BCD code.
 *	val1 is for the correct representation of the bits 4-8.
 *	val2 is a correction factor that ensures that the decimal value 10-15 is not executed.
 */
void count_led( uint8_t *temporary )
{
	uint8_t val1 = (*temporary / 10) % 10;
	uint8_t val2 = *temporary % 10;	
	PORTB = ~( val1 * 16 + val2 );
}

void set_delay( uint16_t delay )
{
	timer = 0;
	while( timer <= delay );
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

