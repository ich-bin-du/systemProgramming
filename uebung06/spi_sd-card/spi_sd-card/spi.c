/*
 * spi.c
 *
 * Created: 04.04.2021 16:06:27
 *  Author: DAmth
 */ 

 #include <avr/io.h>
 #include <stdio.h>

 void init_spi_master()
 {
	 DDRB = (1 << MOSI) | (1 << SCK) | (1 << SS);	// Init MOSI, SCK and SS as Output and MISO as Input
	 SPCR = (1 << SPE) | (1 << MSTR);	// SPI Control Register: activate SPE (SPI enable) and set MSTR -> Master Mode (Master/Slave select)
	 
	 printf( "Init SPI complete!\n" );
 }

 void transmit_data_master( unsigned char data )
 {
	 SPDR = data;	// SPI Data Register: Write data byte to SPDR to start data transfer

	 while( !(SPSR & (1 << SPIF)) );	// when data transfer complete SPI Interrupt Flag is set in SPI Status Register
 }