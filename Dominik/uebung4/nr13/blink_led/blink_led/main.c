/*
 * blink_led.c
 *
 * Created: 21.03.2021 13:33:57
 * Author : Dominik Amthor
 */ 

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>

/************************************************************************/
/*  Initialisierung LEDs als Output                                     */
/*  Initialisierung alle LEDs auﬂer die erste auf High (LEDs active low)*/
/************************************************************************/
void init()
{
	DDRB = (1 << PB0) | (1 << PB1);
	PORTB = 0xfe; //LED ist Active Low
}

/************************************************************************/
/*  Abwechseln der LEDs von High to Low                                 */
/************************************************************************/
void toggle_leds()
{
	PORTB = (1 << PB0);
	_delay_ms( 1000 );
	PORTB = (1 << PB1);
	_delay_ms( 1000 );
}

int main( void )
{
	init();

    while ( 1 ) 
    {
		toggle_leds();
    }
}
