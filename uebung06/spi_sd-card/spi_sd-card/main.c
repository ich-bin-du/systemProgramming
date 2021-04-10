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

void init_leds()
{
	DDRA = 0xff;	// LED Output for debugging
	PORTA = 0xff;	// LEDs active low
}

int main( void )
{
	init_leds();

	usart_init();
	usart_setup_stdio_stream();

    spi_init_master();
	sd_init();

	uint8_t buffer[512];
	for( int i = 512; i > 0; i-- )
		buffer[i] = i;
	buffer[511] = 0x00;

	sd_write_single_block( buffer );

	uint8_t buf[512];
	sd_read_single_block( buf );

	for( int i = 0; i < 512; ++i )
	{
		PORTA = ~buf[i];
		printf( "0x%x ", PINA );
	}

    while (1) 
    {
    }
}

