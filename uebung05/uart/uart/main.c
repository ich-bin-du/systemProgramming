/*
 * main.c
 *
 * Created: 28.03.2021 15:48:13
 * Author : Dominik Amthor
 */ 

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"

void init()
{
	DDRB = 0xff;	// LED PortB as Output
	PORTB = 0xff;	// LEDs are Active Low therefore Pull Up
}

void send_text( char msg[], uint8_t* length )
{
	for( int i = 0; i < *length; ++i )
	{
		transmit_usart( msg[i] );
	}
}

char count_led( uint8_t* count )
{
	*count = *count + 1;
	return *count;
}

int main( void )
{
	init();
	init_usart( 51 );

	//uint8_t count = 0;
	char msg[] = "Das ist ein Test!";
	uint8_t length = sizeof(msg) / sizeof(msg[0]);

    while( 1 ) 
    {
		
		//transmit_usart( ~count_led( &count ) );
		if( PINA != 0xff )
		{
			send_text( msg, &length );
			//transmit_usart( PINA );
		}
    }
}

ISR( USART_RXC_vect )
{
	PORTB = receive_usart();
}

