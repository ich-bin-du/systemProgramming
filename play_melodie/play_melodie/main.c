/*
 * play_melodie.c
 *
 * Created: 22.03.2021 12:08:24
 * Author : DAmth
 */ 

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint16_t timer = 0;


enum noten { C = 523, D = 587, E = 659, F = 698, G = 784, A = 880, B = 988, H = 988, c = 1047 };

const uint16_t entchen_melodie[] = { C, D, E, F, G, G, A, A, A, A, G, A, A, A, A, G, F, F, F, F, E, E, G, G, G, G, C };
const uint8_t entchen_timing[] = { 40, 40, 40, 40, 80, 80, 40, 40, 40, 40, 80, 40, 40, 40, 40, 80, 40, 40, 40, 40, 80, 80, 40, 40, 40, 40, 80 };

void init()
{
	DDRD |= (1 << PD4) | (1 << PD5);
}

void init_timer()
{
	TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1A0) | (1 << COM1B0);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);

	TCCR0 = (1 << WGM01) | (1 << WGM00) | (1 << CS01) | (1 << CS00);
	OCR0 = 125-1;
	TIMSK |= (1 << OCIE0);
	sei();
}


void reset_signal()
{
	OCR1A = 0;
	OCR1B = 0;
}

void set_signal( uint16_t frequency/*, uint16_t tone_length*/ )
{
	uint16_t time = 125000 / frequency; // F_CPU/(prescaler*frequency)
	OCR1A = time;
	OCR1B = time;

}

void set_delay( uint8_t delay )
{
	while( timer <= delay*10 );
	timer = 0;
}


int main( void )
{
	init();
	init_timer();

	uint8_t length = sizeof(entchen_melodie) / sizeof(entchen_melodie[0]);

    while ( 1 )
    {
		for( int i = 0; i < length; i++ )
		{
			set_signal( entchen_melodie[i] );
			set_delay( entchen_timing[i] );
		}
    }
}


ISR( TIMER0_COMP_vect )
{
	timer++;
}