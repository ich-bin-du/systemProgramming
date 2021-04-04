/*
 * usart.c
 *
 * Created: 28.03.2021 15:50:07
 *  Author: Dominik Amthor
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "usart.h"

// Redirect standard output stdout
static FILE mystdout = FDEV_SETUP_STREAM( usart_transmit_printf, NULL, _FDEV_SETUP_WRITE );

/*
 *	Init USART.
 */
void init_usart()
{
	UBRRH = (unsigned char) ( UBBR_BAUD >> 8 );			// Set Baudrate for High Register
	UBRRL = (unsigned char) UBBR_BAUD;					// Set Baudrate for Low Register
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);	// Enable Rx and Tx and Interrupt for Rx
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);	// USREL = 1 to write to URSRC, Set frame 8-Bit Charakter-Size
	sei();												// enable global interrupts
}

/*
 *	Transmit message from sender.
 *
 *	@param data - data to be sent 
 */
void transmit_usart( unsigned char data )
{
	while( !(UCSRA & (1 << UDRE)) );	// UDRE Flag indicates if transmit buffer (UDR) is ready to receive new data

	UDR = data;							// Write data to UDR when UDRE Flag is set
}

/*
 *	Definition of output function. Stream handler for printf.
 *
 *	@param var
 *	@param stream
 */
int usart_transmit_printf( char var, FILE *stream )
{
	transmit_usart( var );
	return 0;
}

/*
 *	Receive message from sender.
 */
unsigned char receive_usart( void )
{
	if( UCSRA & (1 << RXC) );	// RXC Flag is set when unread data is in the receive buffer and cleared when it is empty
		return UDR;		// return received data
	return 0xff;
}

/*
 *	Setup stdio stream.
 */
void setup_stdio_stream( void )
{
	stdout = &mystdout; 
}
