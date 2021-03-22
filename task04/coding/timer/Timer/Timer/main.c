/*
 * Timer.c
 *
 * Created: 21.03.2021 13:46:50
 * Author : Daniel
 */ 

#include <avr/io.h>
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>


void setInitialValues(){
	DDRA = 0x00;		//Input
	DDRB = 0xFF;		//Output
	PORTB=0xFF;
	PINA=0x00;
}

void setBcdCodeTimer(uint8_t seconds){
	uint8_t left = (seconds / 10) % 10; 
	uint8_t right = seconds % 10;
	PORTB = ~(left * 16 + right);
}
void blinking(){
	if(PORTB == 0XFE)
		PORTB = 0XFD;
	else
		PORTB = 0XFE;
}
void showBinary(uint8_t seconds){
	PORTB = ~seconds;
}


int main(void)
{
	setInitialValues();
	TCCR0 = (1<<CS01); // Timer0 configuration, Prescaler = 8
	TIMSK |= (1<<TOIE0); // enable Timer Overflow Interrupt
	sei();
	uint8_t val = 0;
    while (1)	
    {
		/*_delay_ms(1000);
		setBcdCodeTimer(++val);*/
    }
}
uint8_t seconds = 0;
int countInterrupts = 0;
void scale(){
	countInterrupts++;
	if(countInterrupts == (F_CPU / 256 / 8)){
		setBcdCodeTimer(seconds++);
		countInterrupts = 0;
	}
}

ISR(TIMER0_OVF_vect)
{
	/* miscellanous, but keep it short (time) */
	/* Interrupt is called by F_CPU/8/256 Hz */
	scale();
}