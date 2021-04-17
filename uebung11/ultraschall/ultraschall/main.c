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

#define TRIG_PIN		PD3
#define ECHO_PIN		PD2

#define ECHO_SIGNAL		(PIND & (1 << ECHO_PIN))

#define LED_ON			PORTC &= ~(1 << PC0)
#define LED_OFF			PORTC |= (1 << PC0);


static volatile uint8_t signal = 0;
volatile uint32_t timer = 0;
//static volatile uint32_t duration = 0;


void init_int0()
{
	MCUCR |= (1 << ISC00);		// Interrupt 1 Sense Control --> Any logical change on INT1 generates an interrupt request
	GICR |= (1 << INT0);
}



void delay_timer0( void )
{
	TCCR0 = (1 << CS00);				// Normal Mode and no Prescaler
	TCNT0 = 176;
	while( !(TIFR & (1 << TOV0)) );
	TCCR0 = 0;
	TIFR |= (1 << TOV0);
}

//void init_timer0()
//{
	//TCCR0 = (1 << CS00);
	//TCNT0 = 248;
	//TIMSK |= (1 << TOIE0);
//}


/**
 *
 */
void init_pins( void )
{
	DDRD |= (1 << TRIG_PIN);	// Trigger Pin is output and Echo Pin is input (default)
	
	DDRC = 0xff;				// LED Output
	PORTC = 0xff;				// LEDs are Active Low
}


/**
 *
 */
void pulseIn( uint32_t *duration )
{
	
	while( !ECHO_SIGNAL );
	
	timer = 0;
	TCCR0 |= (1 << CS01);
	while( ECHO_SIGNAL )
	{
		//_delay_ms( 58 );
		//*duration = *duration + 1;
		
	}
	TCCR0 &= ~(1 << CS01);
}


void send_trigger_signal()
{
	PORTD |= (1 << TRIG_PIN);
	delay_timer0();
	PORTD &= ~(1 << TRIG_PIN);	
}



int main( void )
{
	usart_init();
	
    init_pins();
	
	//init_int0();
	
	//TCCR1A = 0x00;
	
	TIMSK |= (1 << TOIE0);
	
	uint32_t distance = 0;
	
	//init_timer0();
	
	sei();
	
    while ( 1 ) 
    {	
		uint32_t duration = 0;
		
		send_trigger_signal();
			
		pulseIn( &duration );
		
		distance = ( ((timer<<8) + TCNT0) / 2 )  * 0.03435; /*(duration / 2) / 29.1;*/
		printf( "%lu cm\n", distance );
		
		_delay_ms( 500 );		
    }
}


ISR( TIMER0_OVF_vect )
{
	cli();
	timer++;
	sei();
}

//ISR( INT0_vect )
//{
	////PORTC ^= (1 << PC7);	
	//if( signal == 0 )
	//{
		//TCCR1B |= (1 << CS11);
		//signal = 1;
	//}
	//else if( signal == 1 )
	//{
		//TCCR1B = 0x00;
		//duration = TCNT1;
		////printf( "%lu cm\n", duration );
		//TCNT1 = 0;
		//signal = 0;
	//}
//}