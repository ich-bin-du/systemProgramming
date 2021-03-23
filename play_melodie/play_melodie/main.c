/*
 * play_melodie.c
 *
 * Created: 22.03.2021 12:08:24
 * Author : Dominik Amthor
 */ 

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint16_t timer = 0;

// Music notes 
enum music_notes { C = 523, D = 587, E = 659, F = 698, G = 784, A = 880, B = 988, H = 988, c = 1047 };

// Melodie and Timing for "Alle meine Entchen"
const uint16_t entchen_melodie[] = { C, D, E, F, G, G, A, A, A, A, G, A, A, A, A, G, F, F, F, F, E, E, G, G, G, G, C };
const uint8_t entchen_timing[] = { 40, 40, 40, 40, 80, 80, 40, 40, 40, 40, 80, 40, 40, 40, 40, 80, 40, 40, 40, 40, 80, 80, 40, 40, 40, 40, 80 };

/**
 *	Initiate PD4 (OC1B) and PD5 (OC1A) as Output.
 */
void init()
{
	DDRD |= (1 << PD4) | (1 << PD5); // OC1B and OC1A
}

/**
 *	Initiate 16-Bit Timer/Counter1 and 8-Bit Timer/Counter0.
 */
void init_timer()
{
	TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1A0) | (1 << COM1B0);	// activate Fast PWM (WGM13-10) and toggle OC1A (COM1A0 and COM1B0) on Compare Match
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);		// Prescaler 64

	TCCR0 = (1 << WGM01) | (1 << WGM00) | (1 << CS01) | (1 << CS00);	// activate Fast PWM (WGM01-WGM00)
	OCR0 = 125-1;			// Prescaler 64 (8000000Hz/Prescaler)/1000ms) = 125
	TIMSK |= (1 << OCIE0);	// Timer Interrupt Mask Register: activate Compare Interrupt
	sei();					// activate Global Interrupts
}

/**
 *	Generate specific Output Compare Match Value.
 */
void set_signal( uint16_t frequency )
{
	uint16_t time = (uint32_t) 8000000 / (64 * frequency); // F_CPU/(prescaler*frequency)
	OCR1A = time;
	OCR1B = time;
}

/**
 *	Set delay for each music note.
 */
void set_delay( uint8_t delay )
{
	while( timer <= delay*6 );
	timer = 0;
}

/**
 *	Play melodie "Alle meine Entchen".
 */
void play_melodie_entchen( uint8_t *length )
{
	for( int i = 0; i < *length; i++ )
	{
		set_signal( entchen_melodie[i] );
		set_delay( entchen_timing[i] );
	}
}

/**
 *	Returns the length of the melodie array.
 */
uint8_t get_melodie_length( )
{
	return sizeof(entchen_melodie) / sizeof(entchen_melodie[0]);
}


int main( void )
{
	init();
	init_timer();

	uint8_t length = get_melodie_length();
	
    while ( 1 )
    {
		play_melodie_entchen( &length );
    }
}


ISR( TIMER0_COMP_vect )
{
	timer++;
}