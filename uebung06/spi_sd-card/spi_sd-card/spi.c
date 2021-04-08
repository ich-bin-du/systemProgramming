/*
 * spi.c
 *
 * Created: 04.04.2021 16:06:27
 *  Author: Dominik Amthor
 */ 

 #include <avr/io.h>
 #include <stdio.h>
 #include "spi.h"

/**
 *	Initialize SPI Pins and Registers.
 */
void spi_init_master( void )
{
	// Init MOSI, SCK and SS as Output and MISO as Input
	DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS);

	// SPI Control Register: activate SPE (SPI enable) and set MSTR -> Master Mode (Master/Slave select) and set clock rate fosc/64
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);

	printf( "Init SPI complete!\n" );
}

/**
 *	Write data to SPI Data Register and transmit to receiver.
 *
 *	@param data	data byte which is to be transmitted.
 */
void spi_transmit_data( uint8_t data )
{
	SPDR = data;	// SPI Data Register: Write data byte to SPDR to start data transfer

	while( !(SPSR & (1 << SPIF)) );	// when data transfer complete SPI Interrupt Flag is set in SPI Status Register
}

/**
 *	Receive byte which was transmitted via spi. 
 *
 *	@return uint8_t SPI Data Register with received data.
 */
uint8_t spi_receive_data( void )
{
	while( !(SPSR & (1 << SPIF)) );	// Wait until message is in the register

	return SPDR;
}

/**
 *	SS is negated. To activate the pin it must be deleted.
 */
void spi_enable_slave_select( void )
{
	PORTB &= ~(1 << SS);
}

/**
 *	SS is negated. To disable the pin it must be set.
 */
void spi_disable_slave_select( void )
{
	PORTB |= (1 << SS);
}
