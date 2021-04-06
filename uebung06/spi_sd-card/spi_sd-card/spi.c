/*
 * spi.c
 *
 * Created: 04.04.2021 16:06:27
 *  Author: DAmth
 */ 

 #include <avr/io.h>
 #include <stdio.h>
 #include "spi.h"

void spi_init_master()
{
	DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS);	// Init MOSI, SCK and SS as Output and MISO as Input
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);	// SPI Control Register: activate SPE (SPI enable) and set MSTR -> Master Mode (Master/Slave select) and set clock rate fosc/64
	 
	printf( "Init SPI complete!\n" );
}

void spi_transmit_data( unsigned char data )
{
	SPDR = data;	// SPI Data Register: Write data byte to SPDR to start data transfer

	while( !(SPSR & (1 << SPIF)) );	// when data transfer complete SPI Interrupt Flag is set in SPI Status Register
}

unsigned char spi_receive_data()
{
	while( !(SPSR & (1 << SPIF)) );

	return SPDR;
}

void spi_enable_slave_select( void )
{
	PORTB &= ~(1 << SS);
}

void spi_disable_slave_select( void )
{
	PORTB |= (1 << SS);
}
