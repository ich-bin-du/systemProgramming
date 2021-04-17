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
#include "pdc.h"


#define TRIG_PIN		PD3
#define ECHO_PIN		PD2

#define ECHO_SIGNAL		(PIND & (1 << ECHO_PIN))


volatile uint32_t duration = 0;


/**
 *	Initialize Trigger pin as output and Echo pin as input.
 */
void init_pins( void )
{
	DDRD |= (1 << TRIG_PIN);	// Trigger pin is output and Echo pin is input (default)
}


/**
 *	Use timer0 in Normal Mode as overflow counter. The delay is set to 10us. 
 *	If the overflow flag (TOV0) is not set then it will wait till overflow 
 *	occurs and TOV0 is set.
 */
void delay_timer0( void )
{
	duration = 0;
	TCCR0 = (1 << CS00);				// Normal Mode and no Prescaler
	TCNT0 = 176;						// (TCNT0-Register - [delay]us) / (1 / 8Mhz) = (256 - 10us) / (1 / 8Mhz) = 176
	while( !(TIFR & (1 << TOV0)) );		// check if TOV0 is set. It is set if overflow in TCNT0 occurs. If it is not set loop.
	TCCR0 = 0;							// deactivate Timer0 (TCCR0)
	TIFR |= (1 << TOV0);				// TOV0 is cleared by writing a logic one to the flag
}


/**
 *	As long as no echo signal is received, the program is waiting. 
 *	The timer0 is used to count up a timer variable. When the echo signal is 
 *	received, the timer starts counting. As soon as the echo signal is low, 
 *	the timer is deactivated.
 */
void pulseIn( void )
{
	
	while( !ECHO_SIGNAL );		// waiting for echo signal
	
	duration = 0;				// reset timer count variable
	TCCR0 |= (1 << CS01);		// init timer0 with prescaler 8 --> (1 / (8Mhz / (8 * 256))) = 256us => time between two overflows
	
	while( ECHO_SIGNAL );		// wait while timer0 is counting till echo is low
	
	TCCR0 &= ~(1 << CS01);		// deactivate Timer0 (TCCR0)
}


/**
 *	Send high level trigger signal for 10us with timer0.
 *				 | 10us |
 *				  ______
 *	Trigger		 |	    |	
 *	Signal	_____|	    |________
 */
void send_trigger_signal( void )
{
	PORTD |= (1 << TRIG_PIN);
	delay_timer0();
	PORTD &= ~(1 << TRIG_PIN);	
}



int main( void )
{
	usart_init();			// Init usart from usart.h lib
	
	init_pins();			// Init ultrasonic pins
	
	speaker_init();			// Init speaker from pdc.h lib
	
	TIMSK |= (1 << TOIE0);	// Preparation Timer/Counter0 Overflow Interrupt Enable
	
	uint32_t distance = 0;	// Declaration of distance var
	
	sei();					// enable global interrupts
	
	while ( 1 )
	{
		send_trigger_signal();	
		
		pulseIn();
		
		/* Duration is the time between the receiving of the echo signal. The duration variable is 
		   incremented in the timer overflow interrupt when the echo signal is received and counts 
		   the number of overflows. The left shift by 8 corresponds to a multiplication by 256 
		   (size of the TCNT0 counter register). This then represents the reception time in us. 
		   The rest that is still in the TCNT0 register can then simply be added.Since you only 
		   want the simple send distance (there without back), you divide by 2.  The factor 0.03435 
		   corresponds to the Air spreading velocity [ cAir ? (331,5 + 0,6 ambient temp. / °C) m/s ].
		*/
		distance = ( ((duration<<8) + TCNT0) / 2 )  * 0.03435;	
		
		speaker_distance_sound( distance );		// call speaker method from pdc.h
		
		printf( "%lu cm\n", distance );			// simply print distance value
		
		_delay_ms( 50 );						//TODO Delay weg
	}
}


/**
 *	Timer0 overflow ISR. Counts while receiving the echo signal.
 */
ISR( TIMER0_OVF_vect )
{
	cli();
	duration++;
	sei();
}