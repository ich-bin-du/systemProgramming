/*
 * usart.h
 *
 * Created: 28.03.2021 16:22:57
 *  Author: Dominik Amthor
 */ 

#ifndef USART_H_
#define USART_H_

#include <stdio.h>

#define F_CPU 8000000UL
#define BAUD		9600UL
#define UBBR_BAUD	(F_CPU/(BAUD*16)-1)


void init_usart( void );

void transmit_usart( unsigned char data );

int usart_transmit_printf( char var, FILE *stream );

unsigned char receive_usart( void );

void setup_stdio_stream( void );

#endif /* USART_H_ */