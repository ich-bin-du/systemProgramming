/*
 * FirstSample.c
 *
 * Created: 15.03.2021 13:32:36
 * Author : Daniel
 */ 

//#define PINA (*(volatile uint8_t*)(0x10))
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

enum buttonState {
	buttonNotPressed = 0,
	buttonPressed = 1,
};

void changeOutput(){
	PORTB--;
}

enum buttonState currentState = buttonNotPressed;

int isInputNotDefaultValue(){
	return PINA != 0xFF;
}

void setState(){
	if(isInputNotDefaultValue())
		currentState = buttonPressed;
	else 
		currentState = buttonNotPressed;
}

int buttonInput(){
	if( currentState == buttonNotPressed && isInputNotDefaultValue() ){
		currentState = buttonPressed;
		return 1;
	}
	setState();
	return 0;
}

void setInitialValues(){
	DDRA = 0x00;		//Input
	DDRB = 0xFF;		//Output
	PORTB=0xFF;
	PINA=0x00;
}

int main(void)
{
	setInitialValues();
    while (1) 
    {
		if(buttonInput())
			changeOutput(); 
    }
}