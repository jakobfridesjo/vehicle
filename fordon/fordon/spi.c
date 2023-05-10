/*
 * spi.c
 *
 * Created: 2021-05-10
 * Author : Jakob Fridesjo
 */ 

#include <avr/io.h>
#include "spi.h"
#include "usart.h"

/************************************************************************/
/* @brief Initializes the SPI                                                  */
/************************************************************************/
void spi_master_init(void) {
	
	// SCK, MOSI, SS as outputs
	DDRB  |= (1<<2) | (1<<3)| (1<<5) | (1<<1) | (1<<0);
	// MISO as input
	DDRB  &= ~(1<<4);   
	// Set Display low                        
	PORTB &= ~(1<<1);	
	// Enable master
	SPCR |= (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

/************************************************************************/
/* @brief Sends byte using SPI                                          */
/* @byte The byte to be sent over SPI                                   */
/* @return The byte received from SPI                                   */
/************************************************************************/
uint8_t spi_txrx(uint8_t byte) {
	uint8_t rx = 0;
	// send the data
	SPDR = byte;
	while(!(SPSR & (1<<SPIF)));
	//return received data
	rx = SPDR;
	return rx;
}