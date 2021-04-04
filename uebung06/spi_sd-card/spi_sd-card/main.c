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

    while (1) 
    {
    }
}

