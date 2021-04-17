/*
 * pdc.c
 *
 * Created: 17.04.2021 17:23:22
 *  Author: Dominik Amthor
 */ 

#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pdc.h"



void speaker_init()
{
	DDRD |= (1 << PD4) | (1 << PD5);	// PD4 and PD5 pins are OC1B and OC1A from 16bit-Timer1
	
	speaker_init_timer1();
}


void speaker_init_timer1()
{
	TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1A0) | (1 << COM1B0);	// FastPWM; Toggle OC1A/OC1B on compare match
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);		// Prescaler 64 
	
	sei();					// enable global interrupts
}


void speaker_set_signal( uint32_t frequency )
{
	uint16_t timing =  ( (uint32_t) F_CPU / (64 * frequency) ) - 1;	// F_CPU/Prescaler/1000ms
	OCR1A = timing;
	OCR1B = timing;
}


void my_delay( uint32_t delay )
{
	for( int i = 0; i < delay; i++ )
	{
		_delay_ms( 1 );
	}
}


void speaker_play_peep( uint32_t delay )
{
	speaker_set_signal( delay );
	my_delay( delay );
	//speaker_set_signal( D5 );
	//my_delay( 1 );
	//speaker_set_signal( SILENCE );
	//my_delay( 100 );
}



void speaker_distance_sound( uint32_t distance )
{
	if( distance >= 0 && distance <= 20 )
		speaker_set_signal( 700 );
	else {
		speaker_play_peep( (distance * 700) / 400 );
	}
}



//int main( void )
//{
	//speaker_init_pins();
	//
	//speaker_init_timer1();
	//
	//while( 1 )
	//{
		//speaker_distance_sound( 55 );
	//}
//}