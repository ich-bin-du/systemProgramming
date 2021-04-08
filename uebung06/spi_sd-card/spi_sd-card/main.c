/*
 * main.c
 *
 * Created: 04.04.2021 11:58:52
 * Author : Dominik Amthor
 */ 
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
	usart_init();
	usart_setup_stdio_stream();

    spi_init_master();
	sd_init();

	//unsigned char msg[] = "Das ist ein Test, ob die Daten auch richtig gespeichert werden.";

	uint8_t buffer[512];
	for( int i = 512; i > 0; i-- )
		buffer[i] = i;

	sd_write_single_block( buffer );

	uint8_t buf[512];
	sd_read_single_block( buf );

	for( int i = 0; i < 512; ++i )
		printf( "0x%02x ", buf[i] );
//
	////printf( "%s", buffer );
//
	//unsigned char status = sd_write_single_block( buffer );
	//printf( "0x%02x", status );
//
	//unsigned char *buf;
	//if( status != 0xe0 )
	//{
		//*buf = sd_read_single_block();
		//printf( "%s", buf);
	//}

    while (1) 
    {
    }
}

