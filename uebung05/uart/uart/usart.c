/*
 * usart.c
 *
 * Created: 28.03.2021 15:50:07
 *  Author: Dominik Amthor
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"

void init_usart( unsigned int baud )
{
	UBRRH = (unsigned char) ( baud >> 8 );	// Set Baudrate for High Register
	UBRRL = (unsigned char) baud;				// Set Baudrate for Low Register
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);		// Enable Rx and Tx and Interrupt for Rx
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);		// USREL = 1 to write to URSRC, Set frame 8-Bit Charakter-Size
	sei();	// enable global interrupts
}

void transmit_usart( unsigned char data )
{
	while( !(UCSRA & (1 << UDRE)) );	// UDRE Flag indicates if transmit buffer (UDR) is ready to receive new data
	
	UDR = data;				// Write data to UDR when UDRE Flag is set
}

unsigned char receive_usart( void )
{
	if( UCSRA & (1 << RXC) );	// RXC Flag is set when unread data is in the receive buffer and cleared when it is empty
		return UDR;		// return received data
	return 0xff;
}
