/*
 * main.c
 *
 * Created: 04.04.2021 11:58:52
 * Author : Dominik Amthor
 */ 

#include <avr/io.h>

void init_spi_master()
{
	DDRB = (1 << DDB5) | (1 << DDB7) | (1 << DDB4);

	SPCR = (1 << SPE) | (1 << MSTR)
}

int main( void )
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

