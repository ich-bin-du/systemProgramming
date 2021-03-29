/*
 * blink_led_bcd.c
 *
 * Created: 21.03.2021 19:48:42
 * Author : Dominik Amthor
 */ 

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

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

int main( void )
{
	init();
	uint8_t temporary = 0;

	while ( 1 )
	{
		temporary++;
		count_led( &temporary );
		_delay_ms( 1000 );
	}
}

