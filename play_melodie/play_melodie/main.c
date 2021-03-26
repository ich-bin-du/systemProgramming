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
enum music_notes { C= 523, C2= 261, D= 587, D4= 293, D5= 587, E = 659, F = 698, F2 = 349, G = 784, G4=391, A = 880, A4= 440, B = 988, B4= 493, H = 988, c = 1047, Eb = 622, Eb2= 311, Bb = 466, Ab = 415 };

// Melodie and Timing for "Alle meine Entchen"
const uint16_t entchen_melodie[] = { C, D, E, F, G, G, A, A, A, A, G, A, A, A, A, G, F, F, F, F, E, E, G, G, G, G, C };
const uint8_t entchen_timing[] = { 40, 40, 40, 40, 80, 80, 40, 40, 40, 40, 80, 40, 40, 40, 40, 80, 40, 40, 40, 40, 80, 80, 40, 40, 40, 40, 80 };

// Melodie and Timing for "Ich geh mit meiner Laterne"
const uint16_t laterne_melodie[] = {
	D4, G4, G4, B4, G4, B4, D5, B4, G4, A4, A4, A4, A4, B4, A4, G4, 
	B4, D5, B4, G4, B4, D5, B4, G4, G4, A4, A4, A4, A4, B4, A4, G4
};
const uint8_t laterne_timing[] = {
	20, 40, 20, 20, 20, 20, 50, 40, 20, 20, 20, 20, 20, 20, 50, 20,
	20, 40, 20, 40, 20, 40, 20, 40, 20, 20, 20, 20, 20, 20, 20, 50
};

// Melodie and Timing for "Game of Thrones"
const uint16_t got_melodie[] = { 
		G, C, Eb, F, G, Eb, F, D,
		F, Bb, Eb, D, F, Bb, Eb, D, C,
		Ab, F2, C2, F2,   G4, C2,
		C2, D4, Eb2, F2, G4,
		};
const uint8_t got_timing[] = { 
		30, 30, 40, 30, 30, 40, 60, 50,
		40, 60, 40, 40, 40, 20, 70, 40,
		30, 30, 30, 60,   60, 80,
		30, 30, 40, 30, 60
		};

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
	timer = 0;
	while( timer <= delay*10 );
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
 *	Play melodie "GoT".
 */
void play_melodie_got( uint8_t *length )
{
	for( int i = 0; i < *length; i++ )
	{
		set_signal( got_melodie[i] );
		set_delay( got_timing[i] );
	}
}

/**
 *	Play melodie "Laterne".
 */
void play_melodie_laterne( uint8_t *length )
{
	for( int i = 0; i < *length; i++ )
	{
		set_signal( laterne_melodie[i] );
		set_delay( laterne_timing[i] );
	}
}

/**
 *	Returns the length of the melodie array.
 */
uint8_t get_melodie_length()
{
	return sizeof(laterne_melodie) / sizeof(laterne_melodie[0]);
}


int main( void )
{
	init();
	init_timer();

	uint8_t length = get_melodie_length();
	
    while ( 1 )
    {
		//play_melodie_entchen( &length );
		play_melodie_laterne( &length );
		//play_melodie_got( &length );
    }
}


ISR( TIMER0_COMP_vect )
{
	timer++;
}