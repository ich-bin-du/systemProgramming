/*
 * Sound.c
 *
 * Created: 22.03.2021 12:08:45
 * Author : Daniel
 */ 


#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


int main(void)
{
	PORTD = 0xFF;
	DDRD = 0xFF;
    /* Replace with your application code */
    while (1) 
    {
		PORTD--;
		_delay_ms(100);
    }
}
