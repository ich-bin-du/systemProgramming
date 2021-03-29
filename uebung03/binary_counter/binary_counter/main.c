/*
 * binary_counter.c
 *
 * Created: 16.03.2021 18:07:15
 * Author : Dominik Amthor
 */ 

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

uint8_t btnState = 0;

/************************************************************************/
/*		  ___															*/		
/*	 ____|	 |____														*/
/*       0 1 2 3														*/
/*	Entprellen des Buttons												*/
/*  Zustand 0: Steigende Flanke bedeutet Button wird gedrückt			*/
/*	Zustand 1: High bedeutet Button wird weiterhin gedrückt				*/
/*	Zustand 2: Fallende Flanke bedeutet Button wird losgelassen			*/
/*	Zustand 3: Low bedeutet Button wird nicht mehr gedrückt				*/
/*																		*/
/*	\return		1, wenn Button gedrückt, ansonsten 0		            */
/************************************************************************/
uint8_t isButtonPressed()
{
	uint8_t btnValue = PINA != 0xFF;

	if ( btnState == 0 && btnValue == 1 ) {
		btnState = 1;
		return 1;
	} else if ( btnState == 1 && btnValue == 1 ) {
		btnState = 2;
		return 0;
	} else if ( btnState == 2 && btnValue == 0 ) {
		btnState = 3;
		return 0;
	} else if ( btnState == 3 && btnValue == 0 ) {
		btnState = 0;
		return 0;
	}
	return 0;
}

void init()
{
	DDRA = 0x00;	// Port A input
	DDRB = 0xff;	// Port B output
	PORTB = 0xff;	// Pull Up
}


void count_led( uint8_t *temporary )
{
	if ( isButtonPressed() )
	{
		*temporary = *temporary + 0x01;
		PORTB = ~*temporary;
	}
}

int main(void)
{
	init();
	uint8_t temporary = ~PORTB;

    while (1) 
    {
		count_led( &temporary );
    }
}

