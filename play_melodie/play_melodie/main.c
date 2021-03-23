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
	//for( int i = 0; i < tone_length; i++ )
	//{
		OCR1A = time;
		OCR1B = time;
	//}
	//_delay_ms( 100 );
}

void set_delay( uint8_t delay )
{
	while( timer <= delay*10 );
	timer = 0;
	//for( int i = 0; i < delay; i++ )
		//_delay_ms( 10 );
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

		//set_signal( C ); // 1
		//set_delay( 40 );
//
		//set_signal( D ); // 2
		//set_delay( 40 );
//
		//set_signal( E ); // 3
		//set_delay( 40 );
//
		//set_signal( F ); // 4
		//set_delay( 40 );
//
		//set_signal( G ); // 5
		//set_delay( 80 );

		//set_signal( G ); // 6
		//set_delay( 80 );
//
		//set_signal( A ); // 7
		//set_delay( 40 );
//
		//set_signal( A ); // 8
		//set_delay( 40 );
//
		//set_signal( A ); // 9
		//set_delay( 40 );
//
		//set_signal( A ); // 10
		//set_delay( 40 );
//
		//set_signal( G ); // 11
		//set_delay( 80 );

		//set_signal( A ); // 12
		//set_delay( 40 );
//
		//set_signal( A ); // 13
		//set_delay( 40 );
//
		//set_signal( A ); // 14
		//set_delay( 40 );
//
		//set_signal( A ); // 15
		//set_delay( 40 );
//
		//set_signal( G ); // 11
		//set_delay( 80 );

		//set_signal( F ); // 16
		//set_delay( 40 );
//
		//set_signal( F ); // 13
		//set_delay( 40 );
//
		//set_signal( F ); // 14
		//set_delay( 40 );
//
		//set_signal( F ); // 15
		//set_delay( 40 );
//
		//set_signal( E ); // 16
		//set_delay( 80 );

		//set_signal( E ); // 17
		//set_delay( 80 );
//
		//set_signal( G ); // 18
		//set_delay( 80 );
//
		//set_signal( G ); // 19
		//set_delay( 80 );
//
		//set_signal( G ); // 20
		//set_delay( 80 );
//
		//set_signal( G ); // 21
		//set_delay( 80 );
//
		//set_signal( C ); // 21
		//set_delay( 80 );

    }
}


ISR( TIMER0_COMP_vect )
{
	timer++;
}