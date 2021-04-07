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

unsigned char CMD16[] = { 0x50, 0x00, 0x00, 0x02, 0x00, 0x01 };		// SET_BLOCKLEN (Blocksize 512 Bytes)

unsigned char CMD17[] = { 0x51, 0x00, 0x00, 0x08, 0x00, 0x01 };		// READ_SINGLE_BLOCK

unsigned char CMD24[] = { 0x58, 0x00, 0x00, 0x08, 0x00, 0x01 };		// WRITE_SINGLE_BLOCK



unsigned char CMD8[] = { 0x48, 0x00, 0x00, 0x01, 0xaa, 0x87 };		// SEND_IF_COND

unsigned char CMD58[] = { 0x7a, 0x00, 0x00, 0x00, 0x00, 0x01 };		// READ_OCR

unsigned char CMD13[] = { 0x4d, 0x00, 0x00, 0x00, 0x00, 0x01 };		// SEND_STATUS

unsigned char ACMD22[] = { 0x56, 0x00, 0x00, 0x00, 0x00, 0x01 };	// SEND_NUM_WR_BLOCKS

/*
 *	Init SD-Card.
 */
uint8_t sd_init( void )
{
	uint8_t result[5], terminate_counter = 0;

	printf( "Start SD-Card init.\n" );

	DDRB |= (1 << EN1) | (1 << EN2);	// EN1= Reset SD-Card; EN2= Power SD-Card Holder

	sd_holder_hardware_reset();	// apply hardware reset of sd-card holder

	_delay_ms( 250 );	// Chapter 6.4. Power Scheme -> sd-card vdd_min is reached within 250ms

	//spi_set_slave_select();
	//_delay_ms( 5 );	// Chapter 6.4. Power Scheme -> in case of SPI mode CS (Chip Select) shall be held to high during 74 clock cycles -> 1/(F_CPU/Prescaler)*1000 = 1 clock cycle
	//spi_reset_slave_select();
	
	sd_power_up_seq();

	while( (result[0] = sd_go_idle_state()) != 0x01 )
	{
		printf( "Go_IDLE_STATE = 0x%02x \n ", result[0] );
		terminate_counter++;
		if( terminate_counter > 10 ) return 1;
	}
	printf( "-> Result from Go_IDLE_STATE = 0x%02x \n\n ", result[0] );

	_delay_ms(10);

	sd_send_if_cond( result ); // CMD8

		if( result[0] != 0x01 )
		return 1;

		if( result[4] != 0xaa )
		return 1;

	terminate_counter = 0;
	do 
	{
		if( terminate_counter > 100 ) return 1;

		result[0] = sd_send_app_cmd();	// CMD55

		if( result[0] < 2 )
			result[0] = sd_send_op_cond(); // ACMD41

		_delay_ms( 10 );

		terminate_counter++;

	} while ( result[0] != 0x00 );

	_delay_ms(10);

	sd_send_read_ocr( result ); // CMD58

	return 0;
	//spi_transmit_data( 0xff );
	//spi_enable_slave_select();
	//spi_transmit_data( 0xff );
//
	//printf( "Sending the CMD0 command\n" );
	////spi_set_slave_select();		// start SPI mode
	//do 
	//{
		//received_status = sd_send_cmd( CMD0 );	// Chapter 6.4. Power Scheme -> In case of SPI host, CMD0 shall be the first command to send the card to SPI mode.
	//} while( received_status != 0x01 );
	
	//spi_transmit_data( 0xff );
	//spi_disable_slave_select();
	//spi_transmit_data( 0xff );
//
	//spi_transmit_data( 0xff );
	//spi_enable_slave_select();
	//spi_transmit_data( 0xff );
//
	//printf( "Sending the CMD8 command\n" );
	//received_status = sd_send_cmd( CMD8 );
	//printf( "Response CMD8 0x%02x\n", received_status );
//
	//spi_transmit_data( 0xff ); 
	//received_status = spi_receive_data();	
	//printf( "Response CMD8 0x%02x\n", received_status );

	//printf( "Sending the CMD8 command\n" );
	//received_status = sd_send_cmd( CMD8 );
	//spi_transmit_data( 0xff );
	//received_status = spi_receive_data();
	//printf( "0x%02x\n", received_status );

	//printf( "Sending the CMD55 and ACMD41 command\n" );
	//do 
	//{
		//sd_send_cmd( CMD55 ); // If you send the card a CMD55 (this includes waiting for the response) and then another command, it will be interpreted as an "application specific" command
		//received_status = sd_send_cmd( ACMD41 );
	//} while ( received_status != 0x00 );
//
//
	//printf( "Sending the CMD16 command to set Block length to 512 Bytes.\n" );
	//do
	//{
		//received_status = sd_send_cmd( CMD16 );
	//} while ( received_status != 0x00 );
 }


void sd_power_up_seq( void )
{
	printf( "Begin with Power Up Sequence \n" );

	spi_disable_slave_select();		// make sure card is deselected

	_delay_ms( 10 );				// time for sd card to power up

	spi_transmit_data( 0xff );
	spi_disable_slave_select();		// deselect SD card
	
	for( uint8_t i = 0; i < 80; i++ )	// send 80 clock cycles to synchronize 
		spi_transmit_data( 0xff );	 
}


/*
 *	EN1= Reset SD-Card (low= running mode / high = reset)
 *	EN2= Power SD-Card Holder (low= off / high= on)
 *	Timing signal min. 50ms
 */
void sd_holder_hardware_reset( void )
{
	printf( "Begin with SD-Card Holder reset \n" );

	PORTB |= (1 << EN1);
	PORTB &= ~(1 << EN2);
	_delay_ms( 100 );
	PORTB &= ~(1 << EN1);
	PORTB |= (1 << EN2);
}


/*
 *	
 */
void sd_send_cmd( unsigned char *cmd )
{	
	printf( "Send Command \n" );

	for( uint8_t i = 0; i < 6; ++i )
		spi_transmit_data( cmd[i] );	// transmit 6 byte of CMD (start bit, transmission bit, command index (6bit), argument(32bit), CRC (7bit), end bit
}

/*
 *	
 */
uint8_t sd_read_r1_result( void )
{
	printf( "Start reading R1 result \n" );

	uint8_t terminate_counter = 0, r1_result;

	do {
		spi_transmit_data( 0xff );
		r1_result = spi_receive_data();
		printf( "---> R1-Result = 0x%02x \n", r1_result );

		terminate_counter++;

	} while( (r1_result == 0xff) && (terminate_counter < 8) );

	return r1_result;
}

uint8_t sd_go_idle_state( void )
{
	printf( "Start sending go_idle_state (CMD0) \n" );

	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd( CMD0 );

	uint8_t r1_result = sd_read_r1_result();

	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );

	return r1_result;
}

void sd_read_r7_result( uint8_t *result )
{
	printf( "Start reading R7 result \n" );

	result[0] = sd_read_r1_result();
	printf( "---> R7-Result[0] = 0x%02x \n", result[0] );

	if( result[0] > 1 ) return;

	for( uint8_t i = 1; i < 5; ++i )
	{
		spi_transmit_data( 0xff );
		result[i] = spi_receive_data();
		printf( "---> R7-Result[%i] = 0x%02x \n", i, result[i] );
	}
}

/*
 *	
 */
void sd_send_if_cond( uint8_t *result )
{
	printf( "Start sending if_cond (CMD8) \n" );

	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd( CMD8 );

	sd_read_r7_result( result );

	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );
}


/*
 *	
 */
void sd_read_r3_result( uint8_t *result )
{
	printf( "R3-Result is called \n" );

	sd_read_r7_result( result );
}

void sd_send_read_ocr( uint8_t *result )
{
	printf( "Start sending read_ocr (CMD58) \n" );

	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd( CMD58 );

	sd_read_r3_result( result );

	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );	
}

uint8_t sd_send_app_cmd( void )
{
	printf( "Start sending app_cmd (CMD55) \n" );

	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd( CMD55 );

	uint8_t r1_result = sd_read_r1_result();

	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );

	return r1_result;
}

uint8_t sd_send_op_cond( void )
{
	printf( "Start sending send_op_cond (ACMD41) \n" );

	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd( ACMD41 );

	uint8_t r1_result = sd_read_r1_result();

	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );

	return r1_result;
}


unsigned char* sd_read_single_block( void )
{
	uint8_t r1_result, read;
	uint16_t terminate_counter;
	static unsigned char buffer[512];

	// assert chip select
	spi_transmit_data( 0xFF );
	spi_enable_slave_select();
	spi_transmit_data( 0xFF );

	sd_send_cmd( CMD17 );	// send CMD17
	r1_result = sd_read_r1_result();	// read R1

	// if response received from card
	if( r1_result != 0xff )
	{
		// wait for a response token (timeout = 100ms)
		terminate_counter = 0;
		while( ++terminate_counter != 1563 )
		{
			spi_transmit_data( 0xff );
			if( (read = spi_receive_data() ) != 0xff ) break;
		}
		
		// if response token is 0xFE
		if( read == 0xfe )
		{
			// read 512 byte block
			for( uint16_t i = 0; i < 512; i++ ) 
			{
				spi_transmit_data( 0xff );
				buffer[i] = spi_receive_data(); 
			}

			// read 16-bit CRC
			spi_transmit_data( 0xff );
			spi_transmit_data( 0xff );
		}
	}

	// deassert chip select
	spi_transmit_data( 0xFF );
	spi_disable_slave_select();
	spi_transmit_data( 0xFF );

	return buffer;
}


uint8_t sd_write_single_block( unsigned char* data )
{
	uint8_t r1_result, terminate_counter, read;

	// assert chip select
	spi_transmit_data( 0xFF );
	spi_enable_slave_select();
	spi_transmit_data( 0xFF );

	sd_send_cmd( CMD24 ); // send CMD24
	r1_result = sd_read_r1_result();	// read response

	// if no error
	if( r1_result == 0x00 )
	{
		spi_transmit_data( 0xfe );	// send start token

		// write buffer to card
		for( uint16_t i = 0; i < 512; i++ ) 
			spi_transmit_data( data[i] );

		// wait for a response (timeout = 250ms)
		terminate_counter = 0;
		while( ++terminate_counter != 1563 )
		{
			spi_transmit_data( 0xff );
			read = spi_receive_data();
			if( read != 0xff ) break;
		}
		
		// if data accepted
		if( (read & 0x1f) == 0x05 )
		{
			// wait for write to finish (timeout = 250ms)
			terminate_counter = 0;
			do 
			{
				spi_transmit_data( 0xff );
				if( ++terminate_counter == 1563 ) break;
			} while ( spi_receive_data() == 0x00 );
		}
	}

	// deassert chip select
	spi_transmit_data( 0xFF );
	spi_disable_slave_select();
	spi_transmit_data( 0xFF );

	return r1_result;
}


//unsigned char sd_send_cmd( unsigned char *cmd )
//{
	//unsigned char received_status;
	//uint32_t terminate_counter;
//
	//for( int i = 0; i < 6; ++i )
		//spi_transmit_data( cmd[i] );	// transmit 6 byte of CMD (start bit, transmission bit, command index (6bit), argument(32bit), CRC (7bit), end bit
//
	//terminate_counter = 0;
	//do 
	//{
		//spi_transmit_data( 0xff ); // if you want to read only one byte, e.g. because you are waiting for a response from the card, you simply send a 0xff. The card does the same when it only receives data.
		//received_status = spi_receive_data();	// Generally the card always pulls the MISO line high when it has nothing to say, i.e. you always receive 0xff. 
		//printf( "Send CMD with response 0x%02x\n", received_status );
		//terminate_counter++;
	//} 
	//while( ((received_status & 0x80) != 0x00) && (terminate_counter < 0xffff) ); // wait till MSB in received_data is set. R1 -> 0x01 (in idle state) and timeout
	//// The most Significant Bit (MSB) is transmitted first, the LSB is the last
//
	//return received_status;
//}


/*
 *	
 */
//unsigned char* sd_read_single_block( void )
//{
	//static unsigned char buffer[512];
	//unsigned char received_status;
	//uint16_t terminate_counter;
//
	//printf( "Sending the CMD17 command to read single block from SD.\n" );
	//received_status = sd_send_cmd( CMD17 );	// send cmd 17 to read single block from sd-card
//
	//printf( "Status Code read_single_block 0x%02x.\n", received_status );
	//if( received_status == 0x00 )
	//{
		//terminate_counter = 0;
		//do 
		//{
			//spi_transmit_data( 0xff );
			//received_status = spi_receive_data();
			//printf( "Status Code read_single_block 0x%02x.\n", received_status );
			//terminate_counter++;
		//} while ( (received_status != 0xfe) && (terminate_counter < 0x3ff) );
//
		//printf( "Status Code after read to internal buffer 0x%02x.\n", received_status );
		//if( received_status == 0xfe )			// if data is in internal buffer Data Token 0xfe is send from sd-card
		//{
			//printf( "Read from SD.\n" );
			//for( int i = 0; i < 512; ++i )
			//{
				//spi_transmit_data( 0xff );
				//buffer[i] = spi_receive_data();	// read 512 Bytes data from sd card and save in buffer[] 
			//}
//
			//for( int i = 0; i < 2; ++i )	// read last two CRC bytes after 512 Byte data
			//{
				//spi_transmit_data( 0xff );
				//spi_receive_data();			// read CRC bytes and discard
			//}
			//received_status = 0x01;	
		//}
	//}
	//return buffer;
//}


/*
 *	
 */
//unsigned char sd_write_single_block( unsigned char* data )
//{
	//unsigned char received_status;
	//uint32_t terminate_counter;
//
	//printf( "Sending the CMD24 command to write single block to SD.\n" );
	//received_status = sd_send_cmd( CMD24 );		// To write a data block on sd-card
//
	//if( received_status == 0x00 )
	//{
		//spi_transmit_data( 0xfe );		// Start token, so that the sd-card can recognize the beginning of the block.
		//for( int i = 0; i < 512; ++i )
		//{
			//spi_transmit_data( data[i] );	// write data to specified block from CMD
			////printf( "0x%02x ", spi_receive_data() );
		//}	
//
		//spi_transmit_data( 0xde );		// CRC Byte 1
			////printf( "	0x%02x ", spi_receive_data() );
		//spi_transmit_data( 0xad );		// CRC Byte 2
			////printf( "	0x%02x ", spi_receive_data() );
//
		//terminate_counter = 0;
		//do 
		//{
			//spi_transmit_data( 0xff );
			//received_status = spi_receive_data();
			//printf( "Status Code answer after write 0x%02x.\n", received_status );
			//terminate_counter++;
		//} while ( (received_status == 0xff) && (terminate_counter < 0xffff) );
//
		//printf( "Status 0x%02x.\n", received_status );
		//if( (received_status & 0x1f) == 0x05 )	// when byte is transmitted the sd-card send an Data Response Byte. First three bit should be ignored. 0x05 means 'data accepted'.
		//{
			//terminate_counter = 0;
			//do 
			//{
				//spi_transmit_data( 0xff );
				//received_status = spi_receive_data();	// if status = 0xff the write process is completed.
				//printf( "Receive status 0x%02x\n", received_status );
				//terminate_counter++;
			//} while ( (received_status != 0xff) && (terminate_counter < 0xffff) );
		//}
	//}
	//return received_status;
//}