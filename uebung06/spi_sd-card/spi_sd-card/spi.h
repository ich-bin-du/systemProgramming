/*
 * spi.h
 *
 * Created: 04.04.2021 16:05:43
 *  Author: Dominik Amthor
 */ 


#ifndef SPI_H_
#define SPI_H_

#define MOSI	DDB5
#define SCK		DDB7
#define SS		DDB4

void spi_init_master( void );

void spi_transmit_data( unsigned char data );

unsigned char spi_receive_data( void );

void spi_enable_slave_select( void );

void spi_disable_slave_select( void );

#endif /* SPI_H_ */