/* 
 * main.c
 *
 * Created: 12.04.2021 10:57:20
 * Author : Dominik Amthor
 */ 

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"

#define TRIG_PIN		PD6
#define ECHO_PIN		PD7

#define ECHO_SIGNAL		(PIND & (1 << ECHO_PIN))

#define LED_ON			PORTC &= ~(1 << PC0)
#define LED_OFF			PORTC |= (1 << PC0);


//volatile uint32_t timer0 = 0;


///**
 //*
 //*/
//void init_timer0( void )
//{
	//cli();
	//TCCR0 = (1 << CS00);				// Normal Mode and no Prescaler
	//OCR0 = 8-1;							// 8 Step == 1000 us
	//TIMSK |= (1 << OCIE0);				// enable Output Compare Match
	//sei();								// active Global interrupts
//}


/**
 *
 */
void init_pins( void )
{
	//printf( "Start init of pins \n" );
	DDRD |= (1 << TRIG_PIN);			// Trigger Pin is output and Echo Pin is input (default)
	
	DDRC |= (1 << PC0);					// LED Output
	PORTC |= (1 << PC0);				// LEDs are Active Low
}


/**
 *
 */
void pulseIn( uint32_t *duration )
{
	//printf( "pulseIn is called... \n" );
	
	while( !ECHO_SIGNAL );

	while( ECHO_SIGNAL )
	{
		//_delay_us( 58 );
		*duration = *duration + 1;
	}
}


//void set_delay_us_t0( uint32_t time )
//{
	//timer0 = 0;
	//while( timer0 <= time );
//}


int main( void )
{
	usart_init();
	usart_setup_stdio_stream();
	
    init_pins();
	//init_timer0();
	
    while ( 1 ) 
    {	
		LED_ON;
		
		uint32_t duration = 0, distance = 0;
		
		PORTD &= ~(1 << TRIG_PIN);
		_delay_us( 2 );
		PORTD |= (1 << TRIG_PIN);
		_delay_us( 10 );
		PORTD &= ~(1 << TRIG_PIN);
		
		pulseIn( &duration );
		distance = (duration / 2) * 0.03435; /*(duration / 2) / 29.1;*/
		
		LED_OFF;
		
		printf( "%lu cm\n", distance );
		
		_delay_ms( 1000 );
    }
}

//ISR( TIMER0_COMP_vect )
//{
	//timer0++;
//}


