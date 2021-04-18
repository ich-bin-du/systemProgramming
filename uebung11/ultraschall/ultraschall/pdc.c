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
#include "pdc.h"


// ISR Counter variable.
volatile uint32_t timer_counter = 0;


/**
 *	Initialize Speaker pins and call timer inits.
 */
void speaker_init( void )
{
	DDRD |= (1 << PD4) | (1 << PD5);	// PD4 and PD5 pins are OC1B and OC1A from 16bit-Timer1
	
	speaker_init_timer1();
	speaker_init_timer2();
}


/**
 *	Initialize 16bit-Timer1 with FastPWM and Toggle OC1A/OC1B on compare match.
 *
 */
void speaker_init_timer1( void )
{
	TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1A0) | (1 << COM1B0);	// FastPWM; Toggle OC1A/OC1B on compare match
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);		// Prescaler 64 --> (F_CPU / (64 * 1000ms)) = 125 cycles/ms
	
	sei();					// enable global interrupts
}


/**
 *	Initialize 8bit-Timer2 as _delay_ms replacement. OCR2 is set to 125. That means 125 CPU cycles per ms.
 *	Counter variable timer_counter counts the ms.
 */
void speaker_init_timer2( void )
{
	OCR2 = 125 - 1;			// (F_CPU / (Prescaler * 1000ms)) = 8000000Hz / (64 * 1000ms) = 125 cycle/ms
	TIMSK |= (1 << OCF2);	// enable Timer/Counter2 Output Compare Match Interrupt Enable
	
	sei();					// enable global interrupts
}


/**
 *	Set PWM Signal with specific frequency. The OCR1A and OCR1B output will be triggered with PWM.
 *
 *	@param frequency	Is the frequency of the tone to be played.
 */
void speaker_set_signal( uint32_t frequency )
{
	uint16_t timing =  ( (uint32_t) F_CPU / (64 * frequency) ) - 1;	// F_CPU / (Prescaler*1000ms)
	OCR1A = timing;
	OCR1B = timing;
}


/**
 *	Start timer2 and thus the timer2 comp ISR. The timer_counter variable is incremented 
 *	up to the given delay.
 *
 *	@param delay	Delay value by which delay is to be applied.
 */
void set_delay_ms( uint32_t delay )
{
	TCCR2 = (1 << WGM21) | (1 << CS22);			// CTC Mode; Prescaler 64
	
	for( timer_counter = 0; timer_counter <= delay; );
	
	TCCR2 &= ~(1 << WGM21) | ~(1 << CS22);		// disable timer2 CTC Mode; Prescaler 64
}


/**
 *	Play peep sound with delay.
 *
 *	@param	Delay value by which delay is to be applied.
 */
void speaker_play_peep( uint32_t delay )
{
	speaker_set_signal( delay );
	set_delay_ms( delay );
	//speaker_set_signal( D5 );
	//my_delay( 1 );
	//speaker_set_signal( SILENCE );
	//my_delay( 100 );
}


/**
 *	The passed distance decides which tone will be played. If the distance 
 *	falls below 20cm, then a continuous tone is played.
 *
 *	@param distance		Distance of ultrasonic echo
 */
void speaker_distance_sound( uint32_t distance )
{
	if( distance >= 0 && distance <= 20 )
		speaker_set_signal( 700 );
	else {
		speaker_play_peep( (distance * 700) / 400 );
	}
}


/**
 *	ISR for 8bit-Timer2. Counter for _delay_ms replacement.
 */
ISR( TIMER2_COMP_vect )
{
	timer_counter++;
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