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
	uint8_t status = 0;
	
	init_leds();

	usart_init();
	usart_setup_stdio_stream();

    spi_init_master();
	status = sd_init();
	
	if( status == WRITE_PROTECTION_FAIL )
	{
		printf( "SD-Card is write protected" );
		return 1;
	}
	if( status == CARD_DETECTION_FAIL )
	{
		printf( "No SD-Card inserted." );
		return 1;
	}
	if( status == SD_ERROR )
	{
		printf( "Error during initialization" );
		return 1;
	}
		
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
	

    //while (1) 
    //{
    //}
}

