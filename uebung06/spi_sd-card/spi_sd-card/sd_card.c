/*
 * sd_card.c
 *
 * Created: 04.04.2021 16:25:56
 *  Author: Dominik Amthor
 */ 

 #include <avr/io.h>
 #include <util/delay.h>
 #include "sd_card.h"

 uint8_t CMD0[] = { 0x40, 0x00, 0x00, 0x00, 0x00, 0x95 };

 /*
  *	Init SD-Card.
  */
 void init_sd( void )
 {
	printf( "Start SD-Card init" );

	DDRB = (1 << EN1) | (1 << EN2);	// EN1= Reset SD-Card (low= running mode / high = reset); EN2= Power SD-Card Holder (low= off / high= on); Timing signal min. 50ms
 }
