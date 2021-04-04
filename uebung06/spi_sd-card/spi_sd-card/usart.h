/*
 * usart.h
 *
 * Created: 28.03.2021 16:22:57
 *  Author: Dominik Amthor
 */ 

#ifndef USART_H_
#define USART_H_

#include <stdio.h>

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#define BAUD		9600UL
#define UBBR_BAUD	(F_CPU/(BAUD*16)-1)


void usart_init( void );

void usart_transmit( unsigned char data );

int usart_transmit_printf( char var, FILE *stream );

unsigned char usart_receive( void );

void usart_setup_stdio_stream( void );

#endif /* USART_H_ */