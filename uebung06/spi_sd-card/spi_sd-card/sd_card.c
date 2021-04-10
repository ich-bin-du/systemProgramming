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
#include <stdio.h>
#include <avr/interrupt.h>
#include "sd_card.h"
#include "spi.h"

volatile uint16_t timer = 0;


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
 *
 *	@return uint8_t error state while init.
 */
uint8_t sd_init( void )
{
	uint8_t result[5], terminate_counter = 0;

	init_timer0();			// call timer init

	printf( "Start SD-Card init.\n" );

	printf( "Begin with SD-Card Holder init and reset \n" );
	sd_card_holder_init();	// Init SD-Card holder

	set_delay_ms( 250 );	// Chapter 6.4. Power Scheme -> sd-card vdd_min is reached within 250ms

	printf( "Begin with Power Up Sequence \n" );
	sd_power_up_seq();		// Power-Up sequence to communicate with the sd

	// Try to bring the SD-Card to idle state.
	// Chapter 6.4. Power Scheme -> In case of SPI host, CMD0 shall be the first command to send the card to SPI mode.
	while( (result[0] = sd_cmd_with_r1_response( CMD0 )) != SD_IDLE_STATE )		// CMD0 --> Answer is a R1-Response [SD Spec Chapter 7.3.2.1 Format R1]
	{
		printf( "Go_IDLE_STATE = 0x%02x \n ", result[0] );
		terminate_counter++;
		if( terminate_counter > 10 ) return SD_ERROR;	
	}
	printf( "-> Result from Go_IDLE_STATE = 0x%02x \n\n ", result[0] );

	// Send Interface Condition Command -> Card has to be in idle state -> checks voltage
	// SD Spec Chapter 4.3.13 Send Interface Condition Command (CMD8)
	sd_send_if_cond_CMD8( result );			// CMD8

	if( result[0] != SD_IDLE_STATE )		// answer other than IDLE_STATE leads to an error
		return SD_ERROR;

	if( result[4] != SD_R7_CHECK_PATTERN )	// checks if the echo pattern is the same as what was sent
		return SD_ERROR;

	terminate_counter = 0;
	do 
	{
		if( terminate_counter > 100 ) return SD_ERROR;

		// This command, when received by the card, causes the card to interpret the following command as an application-specific command, ACMD.
		// If you send the card a CMD55 (this includes waiting for the response) and then another command, it will be interpreted as an "application specific" command
		// SD Spec Chapter 4.3.9.1 Application-Specific-Command - APP_CMD (CMD55)
		result[0] = sd_cmd_with_r1_response( CMD55 );		// CMD55

		// SD_SEND_OP_COND (ACMD41) is used to start initialization and to check if the card has completed
		// initialization. It is mandatory to issue CMD8 prior to the first ACMD41
		// SD Spec Chapter 7.2.1 Mode Selection and Initialization and 4.2.3.1 Initialization Command (ACMD41)
		if( result[0] < SD_ERASE_RESET )	// 0x01 means idle and 0x00 means init completed, therefore is all greater than 1 an error state
			result[0] = sd_cmd_with_r1_response( ACMD41 );	// ACMD41

		set_delay_ms( 10 );	// short waiting 
		terminate_counter++;
	} while ( result[0] != SD_INIT_COMPLETED );	// if 0x01 means card is still initializing 0x00 means completion of initialization.

	// Read from OCR Register (operation conditions register)
	sd_send_read_ocr_CMD58( result );	// CMD58

	// Block length of SD-Card can be set with CMD16 (SET_BLOCKLEN. For SDHC and SDXC cards default block length is set to 512 bytes.

	return SD_RUNNING;
}

/**
 *	Initialize the SD-Card Holder Pins and start reset routine of SD-Card Holder.
 *	See Data sheet --> C_CONTROL_PRO_SD_CARD_INTERFACE.pdf
 */
void sd_card_holder_init( void )
{
	DDRB |= (1 << EN1) | (1 << EN2);	// EN1 = Reset SD-Card; EN2 = Power SD-Card Holder
	
	sd_holder_hardware_reset();			// apply hardware reset of SD-Card holder
}


/**
 *	EN1= Reset SD-Card (low= running mode / high = reset)
 *	EN2= Power SD-Card Holder (low= off / high= on)
 *	Timing signal min. 50ms
 *		 ->min.50ms<-
 *			 ___
 *	EN1	____|	|____
 *		____	 ____
 *	EN2		|___|
 */
void sd_holder_hardware_reset( void )
{
	PORTB |= (1 << EN1);
	PORTB &= ~(1 << EN2);
	set_delay_ms( 75 );
	PORTB &= ~(1 << EN1);
	PORTB |= (1 << EN2);
}


/**
 *	Chapter 6.4. Power Scheme
 *	In case of SPI mode CS (Chip Select) shall be held to high during 74 clock cycles -> 1/(F_CPU/Prescaler)*1000 = 1 clock cycle
 *	We need to provide at least 1 ms delay and 74 clock cycles before sending commands to SD Card.
 */
void sd_power_up_seq( void )
{
	spi_disable_slave_select();			// make sure card is deselected (High signal)

	set_delay_ms( 10 );					// time for SD-Card to power up

	spi_transmit_data( 0xff );
	spi_disable_slave_select();			// deselect SD-Card (High signal)
	
	for( uint8_t i = 0; i < 80; i++ )	// send 80 clock cycles to synchronize (8Mhz --> 0,008ms)
		spi_transmit_data( 0xff );	 
}


/**
 *	Initialize 8bit Timer0 in Clear timer on Compare (CTC).
 */
void init_timer0( void )
{
	TCCR0 = (1 << WGM01) | (1 << CS01) | (1 << CS00);	// CTC Mode (Clear timer on Compare); Clock Select -> Prescaler 64
	OCR0 = 125-1;										// Output Compare Register 0CR0 = ((8000000 Hz/64)/1000 ms) = 125 Steps per Second
	TIMSK = (1 << OCIE0);								// Timer Interrupt Mask Register: activate Compare Interrupt
	sei();												// activate Global Interrupts
}


/**
 *	Transmit 6 byte of CMD token
 *	Byte 1:		start bit + transmission bit + command index (6bit)
 *	Byte 2-5:	argument (32bit)
 *	Byte 3:		CRC (7bit) + end bit
 *
 *	@param cmd	command token which should be transfered.
 */
void sd_send_cmd_token( uint8_t *cmd )
{	
	printf( "Send Command \n" );

	for( uint8_t i = 0; i < 6; ++i )
		spi_transmit_data( cmd[i] );	
}


/**
 *	This response token is sent by the card after every command with the exception 
 *	of SEND_STATUS commands. Response length is 1 Byte. MSB is always set to zero.
 *	All other bits indicates an error signaled by 1.
 *	[SD Spec Chapter 7.3.2.1 Format R1]
 *	
 *	@return uint8_t R1 Response token.
 */
uint8_t sd_read_r1_result( void )
{
	printf( "Start reading R1 result \n" );

	uint8_t r1_result, terminate_counter = 0;

	do {
		spi_transmit_data( 0xff );
		r1_result = spi_receive_data();		// run loop till actual data is received
		printf( "---> R1-Result = 0x%02x \n", r1_result );

		terminate_counter++;
	} while( (r1_result == 0xff) && (terminate_counter < 8) );	// abort loop if no response is received for 8 bytes or the result is not equal to 0xff

	return r1_result;
}


/**
 *	This response token is sent by the card when a READ_OCR command is received.
 *	Same length as R7-Result. Therefore R7-Result method is called.
 *	[SD Spec Chapter 7.3.2.1 Format R1]
 *
 *	@param result	Byte array that holds 5 Byte of R3-Response
 */
void sd_read_r3_result( uint8_t *result )
{
	printf( "R3-Result is called \n" );

	sd_read_r7_result( result );	// forward to r7-response method
}


/**
 *	This response token is sent by the card when a SEND_IF_COND command (CMD8) is received. The
 *	response length is 5 bytes. The structure of the first (MSB) byte is identical to response type R1. The
 *	other four bytes contain the card operating voltage information and echo back of check pattern in
 *	argument.
 *	[SD Spec Chapter 7.3.2.6 Format R7]
 *
 *	@param result	Byte array that holds 5 Byte of R7-Response
 */
void sd_read_r7_result( uint8_t *result )
{
	printf( "Start reading R7 result \n" );

	result[0] = sd_read_r1_result();	// 1 byte R1 response token
	printf( "---> R7-Result[0] = 0x%02x \n", result[0] );

	if( result[0] > 1 ) return;		// if R1-Response has an error

	// run loop till the last 4 bytes of R7-Response are received.
	for( uint8_t i = 1; i < 5; ++i )
	{
		spi_transmit_data( 0xff );
		result[i] = spi_receive_data();		
		printf( "---> R7-Result[%i] = 0x%02x \n", i, result[i] );
	}
}


/**
 *	To send commands which receive an R1 response.
 *
 *	@param cmd	command which should be transfered.
 *	@return	uint8_t	R1-Response as result.
 */
uint8_t sd_cmd_with_r1_response( uint8_t *cmd )
{
	printf( "Start sending command with r1 response \n" );

	// Enable Chip Select (CS)
	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd_token( cmd );	// send the parameter that was passed

	uint8_t r1_result = sd_read_r1_result();	// collect R1-Reponse from cmd

	// Disable Chip Select (CS)
	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );

	return r1_result;
}


/**
 *	Send Interface Condition Command -> Card has to be in idle state -> checks voltage
 *	[SD Spec Chapter 4.3.13 Send Interface Condition Command (CMD8)]
 *
 *	@param result	Byte array that holds 5 Byte of R7-Response.
 */
void sd_send_if_cond_CMD8( uint8_t *result )
{
	printf( "Start sending if_cond (CMD8) \n" );

	// Enable Chip Select (CS)
	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd_token( CMD8 );		// send CMD8 command

	sd_read_r7_result( result );	// collect R7-Reponse from CMD8

	// Disable Chip Select (CS)
	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );
}


/**
 *	Read from OCR Register (operation conditions register)
 *	Shows supported voltage of SD-Card and if SD-Card has finished his power-up routine and which card type it is (SDHC/SDXC)
 *	[SD Spec Chapter 7.2.1 Mode Selection and Initialization]
 *
 *	@param result	Byte array that holds 5 Byte of R3-Response.
 */
void sd_send_read_ocr_CMD58( uint8_t *result )
{
	printf( "Start sending read_ocr (CMD58) \n" );

	// Enable Chip Select (CS)
	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd_token( CMD58 );		// send CMD58 command

	sd_read_r3_result( result );	// collect R3-Reponse from CMD58

	// Disable Chip Select (CS)
	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );	
}


/**
 *	The SPI mode supports single block read and Multiple Block read operations (CMD17 or CMD18 in the
 *	SD Memory Card protocol). Upon reception of a valid read command the card will respond with a
 *	response token followed by a data token (Refer to Figure 7-3). In case of Standard Capacity Card, the
 *	size in the data token is determined by the block length set by SET_BLOCKLEN (CMD16). In case of
 *	SDHC and SDXC Cards, block length is fixed to 512 Bytes regardless of the block length set by CMD16.
 *	[SD Spec Chapter 7.2.3 Data Read]
 *
 *	@param buffer		buffer in which the bytes read from the SD card are to be stored.
 *	@return uint8_t		R1-Response of the CMD17.
 */
uint8_t sd_read_single_block( uint8_t *buffer )
{
	uint8_t r1_result, read;
	uint16_t terminate_counter;

	// Enable chip select
	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	// CMD17 --> (READ_SINGLE_BLOCK) initiates a block read and after completing the transfer, the card returns to the Transfer State.
	sd_send_cmd_token( CMD17 );			// send cmd 17 to read single block from sd-card
	r1_result = sd_read_r1_result();	// collect R1 Response from CMD17 command

	// if response received from card
	if( r1_result != 0xff )
	{
		// wait for a response token (timeout = 100ms)
		terminate_counter = 0;
		while( ++terminate_counter != 1563 )
		{
			spi_transmit_data( 0xff );	// if you want to read only one byte, e.g. because you are waiting for a response from the card, you simply send a 0xff. The card does the same when it only receives data.
			if( (read = spi_receive_data()) != 0xff ) break;
		}
		
		// if response token is 0xfe
		if( read == 0xfe )		// if data is in internal buffer Data Token 0xfe is send from sd-card
		{
			// read 512 byte block
			for( uint16_t i = 0; i < 512; i++ ) 
			{
				spi_transmit_data( 0xff );
				buffer[i] = spi_receive_data();		// read 512 Bytes data from sd card and save in buffer[] 
				// Generally the card always pulls the MISO line high when it has nothing to say, i.e. you always receive 0xff. 
			}

			// // read last two CRC bytes after 512 Byte data, 16-bit CRC
			spi_transmit_data( 0xff );
			spi_transmit_data( 0xff );
		}
	}

	// Disable chip select
	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );

	return r1_result;
}


/**
 *	The SPI mode supports single block and multiple block write commands. Upon reception of a valid write
 *	command (CMD24 or CMD25 in the SD Memory Card protocol), the card will respond with a response
 *	token and will wait for a data block to be sent from the host. CRC suffix, block length and start address
 *	restrictions are (with the exception of the CSD parameter WRITE_BL_PARTIAL controlling the partial
 *	block write option and WRITE_BL_LEN) identical to the read operation.
 *
 *	@param data			data to be stored on the SD-Card.
 *	@return uint8_t		R1-Response of the CMD24.
 */
uint8_t sd_write_single_block( uint8_t* data )
{
	uint8_t r1_result, read, terminate_counter = 0;

	// Enable chip select
	spi_transmit_data( 0xff );
	spi_enable_slave_select();
	spi_transmit_data( 0xff );

	sd_send_cmd_token( CMD24 );			// CMD24
	r1_result = sd_read_r1_result();	// R1 Response

	// if no error
	if( r1_result == 0x00 )
	{
		spi_transmit_data( 0xfe );	// // Start token, so that the sd-card can recognize the beginning of the block.

		// write buffer to card
		for( uint16_t i = 0; i < 512; i++ ) 
			spi_transmit_data( data[i] );	// write data to specified block from CMD

		// wait for a response (timeout = 250ms)
		terminate_counter = 0;
		while( ++terminate_counter != 1563 )
		{
			spi_transmit_data( 0xff );
			read = spi_receive_data();	// answer that write was successful or not
			if( read != 0xff ) break;
		}
		
		// if data accepted
		if( (read & 0x1f) == 0x05 )		// when byte is transmitted the sd-card send an Data Response Byte. First three bit should be ignored. 0x05 means 'data accepted'.
		{
			// wait for write to finish (timeout = 250ms)
			terminate_counter = 0;
			do 
			{
				spi_transmit_data( 0xff );
				if( ++terminate_counter == 1563 ) break;
			} while ( spi_receive_data() == 0x00 );		// if response is 0x00 than the write is completed
		}
	}

	// disable chip select
	spi_transmit_data( 0xff );
	spi_disable_slave_select();
	spi_transmit_data( 0xff );

	return r1_result;
}


/**
 *	Own delay method to user 8bit timer with output compare match.
 *	
 *	@param delay	delay in ms.
 */
void set_delay_ms( uint16_t delay )
{
	timer = 0;
	while( timer <= delay );
}

/**
 *	8bit-Timer0 ISR	
 */
ISR( TIMER0_COMP_vect )
{
	timer++;
}