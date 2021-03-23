/*
 * blink_led_delay.c
 *
 * Created: 21.03.2021 21:16:41
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
 *	Count PortB up to show binary second counter. 
 *	If Second 60 is reached reset to 1. 
 */
void count_led( uint8_t *temporary )
{
	if ( *temporary > 0x30 )	// Reset wenn 60 Sekunden erreicht wurden
		*temporary = 0x01;

	PORTB = ~*temporary;
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

