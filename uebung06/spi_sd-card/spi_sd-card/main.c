/*
 * main.c
 *
 * Created: 04.04.2021 11:58:52
 * Author : Dominik Amthor
 */ 

#include <avr/io.h>

void init_spi_master()
{
	DDRB = (1 << DDB5) | (1 << DDB7) | (1 << DDB4);	// Init MOSI, SS and SCK as Output and MISO as Input

	SPCR = (1 << SPE) | (1 << MSTR);	// SPI Control Register: activate SPE (SPI enable) and set MSTR -> Master Mode (Master/Slave select)
}

void transmit_data_master( unsigned char data )
{
	SPDR = data;	// SPI Data Register: Write data byte to SPDR to start data transfer

	while( !(SPSR & (1 << SPIF)) );	// when data transfer complete SPI Interrupt Flag is set in SPI Status Register
}

int main( void )
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

