/*
 * sd_card.c
 *
 * Created: 04.04.2021 16:25:56
 *  Author: Dominik Amthor
 */ 

 #include <avr/io.h>
 #include <util/delay.h>
 #include <stdio.h>
 #include "sd_card.h"

 uint8_t CMD0[] = { 0x40, 0x00, 0x00, 0x00, 0x00, 0x95 };


 /*
  *	Init SD-Card.
  */
void sd_init( void )
{
	printf( "Start SD-Card init" );

	DDRB = (1 << EN1) | (1 << EN2);	// EN1= Reset SD-Card; EN2= Power SD-Card Holder

	_delay_ms( 250 );	// Chapter 6.4. Power Scheme -> sd-card vdd_min is reached within 250ms

	sd_holder_hardware_reset();


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

