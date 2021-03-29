/*
 * count_switch.c
 *
 * Created: 15.03.2021 13:23:43
 * Author : DAmth
 */ 

#include <avr/io.h>


int main(void)
{
    DDRA = 0x00;
	DDRB = 0xff;

    while (1) 
    {
		PORTB = PINA;
    }
}

