/*
 * sd_card.h
 *
 * Created: 04.04.2021 16:25:46
 *  Author: Dominik Amthor
 */ 


#ifndef SD_CARD_H_
#define SD_CARD_H_

// Pins for SD-Card Holder
#define	EN1		PB1			// Reset of SD-Card (low = running mode / high = reset)
#define EN2		PB0			// Power-Up SD-Card Holder (low = off / high = on)

// Command Pins
#define SD_R7_CHECK_PATTERN		0xaa

// Response token state
#define SD_INIT_COMPLETED	0x00
#define SD_IDLE_STATE		0x01
#define SD_ERASE_RESET		0x02

// Status message
#define SD_ERROR	1
#define SD_RUNNING	0


uint8_t sd_init( void );

void sd_card_holder_init( void );

void sd_power_up_seq( void );

void sd_holder_hardware_reset( void );

void init_timer0( void );

void sd_send_cmd_token( uint8_t *cmd );

uint8_t sd_read_r1_result( void );

void sd_read_r3_result( uint8_t *result );

void sd_read_r7_result( uint8_t *result );

uint8_t sd_cmd_with_r1_response( uint8_t *cmd );

void sd_send_if_cond_CMD8( uint8_t *result );

void sd_send_read_ocr_CMD58( uint8_t *result );

uint8_t sd_read_single_block( uint8_t *buffer );

uint8_t sd_write_single_block( uint8_t *data );

void set_delay_ms( uint16_t delay );


#endif /* SD_CARD_H_ */