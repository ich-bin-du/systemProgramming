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


uint8_t sd_init( void );

void sd_card_holder_init( void );

void sd_power_up_seq( void );

void sd_holder_hardware_reset( void );

void sd_send_cmd_token( uint8_t *cmd );

uint8_t sd_read_r1_result( void );

uint8_t sd_go_idle_state_CMD0( void );

void sd_read_r7_result( uint8_t *result );

void sd_send_if_cond( uint8_t *result );

void sd_read_r3_result( uint8_t *result );

void sd_send_read_ocr( uint8_t *result );

uint8_t sd_send_app_cmd( void );

uint8_t sd_send_op_cond( void );

unsigned char* sd_read_single_block( void );

uint8_t sd_write_single_block( unsigned char* data );

//unsigned char* sd_read_single_block( void );

//unsigned char sd_write_single_block( unsigned char data[] );

#endif /* SD_CARD_H_ */