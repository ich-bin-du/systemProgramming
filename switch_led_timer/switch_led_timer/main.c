/*
 * switch_led_timer.c
 *
 * Created: 23.03.2021 10:51:01
 * Author : DAmth
 */ 

 #ifndef F_CPU
	#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t timer;

/**
 *	Initiate PB0 and PB1 as LED Output Pins.
 */
void init()
{
	DDRB = (1 << PB0) | (1 << PB1);
}

/**
 *	Initiate 8-Bit Timer/Counter0.
 */
void init_timer()
{
	TCCR0 = (1 << WGM01) | (1 <<WGM00) | (1 << CS01) | (1 << CS00); // Timer/Counter0 Fast PWM and Prescaler 64
	OCR0 = 125-1;	// Output Compare Register 0 = ((8000000Hz/64Prescaler)/1000ms) = 125 == 1ms
	TIMSK |= (1 << OCIE0);	// Timer Interrupt Mask Register: activate Compare Interrupt
	sei();	// activate Global Interrupts
}

/**
 *	Delay function with Timer. One Step is 1ms.
 */
void set_delay( uint16_t delay )
{
	timer = 0;
	while( timer <= delay );
}

/**
 *	Switch between Led PB0 and Led PB1 every second.
 */
void switch_led()
{
	PORTB = ~(1 << PB0);
	set_delay( 1000 );
	PORTB = ~(1 << PB1);
	set_delay( 1000 );
}


int main( void )
{
	init();
	init_timer();

    while (1) 
    {
		switch_led();
    }
}

ISR( TIMER0_COMP_vect )
{
	timer++;
}

