/*
 * main.c
 *
 * Created: 04.04.2021 11:58:52
 * Author : Dominik Amthor
 */ 
#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include "usart.h"
#include "spi.h"
#include "sd_card.h"

void init()
{
	DDRA = 0xff;	// LED Output for debugging
	PORTA = 0xff;	// LEDs active low
}

int main( void )
{
	init_usart();

	setup_stdio_stream();

    init_spi_master();

    while (1) 
    {
    }
}

