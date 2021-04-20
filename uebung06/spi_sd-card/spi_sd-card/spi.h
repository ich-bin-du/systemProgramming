/*
 * spi.h
 *
 * Created: 04.04.2021 16:05:43
 *  Author: Dominik Amthor
 */ 


#ifndef SPI_H_
#define SPI_H_

#define SS		DDB4
#define MOSI	DDB5
#define MISO	DDB6
#define SCK		DDB7


void spi_init_master( void );

void spi_transmit_data( uint8_t data );

uint8_t spi_receive_data( void );

void spi_enable_slave_select( void );

void spi_disable_slave_select( void );

#endif /* SPI_H_ */