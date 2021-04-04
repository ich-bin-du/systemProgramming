/*
 * usart.h
 *
 * Created: 28.03.2021 16:22:57
 *  Author: Dominik Amthor
 */ 

#ifndef USART_H_
#define USART_H_

void init_usart( unsigned int );

void transmit_usart( unsigned char );

unsigned char receive_usart( void );


#endif /* USART_H_ */