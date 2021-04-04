/*
 * sd_card.c
 *
 * Created: 04.04.2021 16:25:56
 *  Author: Dominik Amthor
 */ 
#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "sd_card.h"
#include "spi.h"


unsigned char CMD0[] = { 0x40, 0x00, 0x00, 0x00, 0x00, 0x95 };		// GO_IDLE_STATE

unsigned char CMD55[] = { 0x77, 0x00, 0x00, 0x00, 0x00, 0x01 };		// APP_CMD

unsigned char ACMD41[] = { 0x69, 0x00, 0x00, 0x00, 0x00, 0x01 };	// SD_SEND_OP_COND

 /*
  *	Init SD-Card.
  */
void sd_init( void )
{
	unsigned char send_status;

	printf( "Start SD-Card init.\n" );

	DDRB |= (1 << EN1) | (1 << EN2);	// EN1= Reset SD-Card; EN2= Power SD-Card Holder

	_delay_ms( 250 );	// Chapter 6.4. Power Scheme -> sd-card vdd_min is reached within 250ms

	sd_holder_hardware_reset();	// apply hardware reset of sd-card holder

	spi_set_slave_select();
	_delay_ms( 5 );	// Chapter 6.4. Power Scheme -> in case of SPI mode CS (Chip Select) shall be held to high during 74 clock cycles -> 1/(F_CPU/Prescaler)*1000 = 1 clock cycle
	spi_reset_slave_select();

	printf( "Sending the CMD0 command\n" );
	spi_set_slave_select();		// start SPI mode
	do 
	{
		send_status = sd_send_cmd( CMD0 );	// Chapter 6.4. Power Scheme -> In case of SPI host, CMD0 shall be the first command to send the card to SPI mode.
	} while ( send_status != 0x01 );

	printf( "Sending the CMD55 and ACMD41 command\n" );
	do 
	{
		sd_send_cmd( CMD55 ); // If you send the card a CMD55 (this includes waiting for the response) and then another command, it will be interpreted as an "application specific" command

		send_status = sd_send_cmd( ACMD41 );
	} while ( send_status != 0x00 );
 }

 /*
  *	EN1= Reset SD-Card (low= running mode / high = reset)
  * EN2= Power SD-Card Holder (low= off / high= on)
  * Timing signal min. 50ms
  */
void sd_holder_hardware_reset( void )
{
	PORTB |= (1 << EN1);
	PORTB &= ~(1 << EN2);
	_delay_ms( 100 );
	PORTB &= ~(1 << EN1);
	PORTB |= (1 << EN2);
}

unsigned char sd_send_cmd( unsigned char *cmd )
{
	unsigned char received_data;
	uint32_t terminate_counter;

	for( int i = 0; i < 6; ++i )
	{
		spi_transmit_data( cmd[i] );	// transmit 6 byte of CMD (start bit, transmission bit, command index (6bit), argument(32bit), CRC (7bit), end bit
	}

	terminate_counter = 0;
	do 
	{
		spi_transmit_data( 0xff ); // if you want to read only one byte, e.g. because you are waiting for a response from the card, you simply send a 0xff. The card does the same when it only receives data.
		received_data = spi_receive_data();	// Generally the card always pulls the MISO line high when it has nothing to say, i.e. you always receive 0xff. 
		printf( "Send CMD with response 0x%02x\n", received_data );
		terminate_counter++;
	} 
	while( ((received_data & 0x80) != 0x00) && (terminate_counter < 0xffff) ); // wait till MSB in received_data is set. R1 -> 0x01 (in idle state) and timeout
	// The most Significant Bit (MSB) is transmitted first, the LSB is the last

	return received_data;
}

