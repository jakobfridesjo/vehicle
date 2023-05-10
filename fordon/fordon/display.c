/*
 * display.c
 *
 * Created: 2021-05-10
 * Author : Jakob Fridesjo
 */ 

#include "display.h"
#include "spi.h"
#include "defines.h"
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

/************************************************************************/
/* @brief Initializes the display                                       */
/************************************************************************/
void display_init(void) {
	display_cs_low();
	// Setup RS register
	DISPLAY_DDR_RS |= (1<<DISPLAY_RS);
	// Set RS Low
	DISPLAY_PORT_RS &= ~(1<<DISPLAY_RS);
	_delay_us(30);
	// Init sequence
	_delay_ms(50);
	spi_txrx(0x39);
	_delay_us(30);
	spi_txrx( 0x1D);
	_delay_us(30);
	spi_txrx(0x50);
	_delay_us(30);
	spi_txrx(0x6C);
	_delay_us(30);
	spi_txrx(0x7C);
	_delay_us(30);
	spi_txrx(0x38);
	_delay_us(30);
	spi_txrx(0x0C);
	_delay_us(30);
	spi_txrx(0x01);
	_delay_ms(1.5);
	spi_txrx(0x06);
	_delay_us(30);
	display_cs_high();
}

/************************************************************************/
/* @brief Clears the display                                            */
/************************************************************************/
void display_clear(void) {
	display_cs_low();
	// Set RS Low
	DISPLAY_PORT_RS &= ~(1<<DISPLAY_RS);
	_delay_us(30);
	// Clear display
	spi_txrx(0x01);
	_delay_ms(1.5);
	display_cs_high();
}

/************************************************************************/
/* @brief Moves the cursor to the right                                 */
/************************************************************************/
void display_move_cursor(uint8_t steps) {
	for (int i = 0; i < steps; i++) {
		display_write_char(' ');
	}
}

/************************************************************************/
/* @brief Writes a char to the display                                  */
/* @c Char to write to display                                          */
/************************************************************************/
void display_write_char(uint8_t c) {
	display_cs_low();
	// Set RS High
	DISPLAY_PORT_RS |= (1<<DISPLAY_RS);
	_delay_us(30);
	// Send to display
	spi_txrx(c);
	display_cs_high();
}

/************************************************************************/
/* @brief Writes a string to the display                                */
/* @string String to write to display                                   */
/* @start Position in string to write from                              */
/* @stop Position in string to write to                                 */
/************************************************************************/
void display_write_string(char *string, uint8_t start, uint8_t stop) {
	for (uint8_t i = start; i < stop; i++) {
		// Send to display
		display_write_char((uint8_t) string[i]);
	}
}

/************************************************************************/
/* @brief Moves the cursor to default position                          */
/************************************************************************/
void display_cursor_home(void) {
	display_cs_low();
	// Set RS Low
	DISPLAY_PORT_RS &= ~(1<<DISPLAY_RS);
	_delay_us(30);
	uint8_t byte = 0x02;
	spi_txrx(byte);
	_delay_ms(1.5);
	display_cs_high();
}

/************************************************************************/
/* @brief Sets display CS high                                          */
/************************************************************************/
void display_cs_high(void) {
	DISPLAY_PORT |= (1<<DISPLAY_CS);
}

/************************************************************************/
/* @brief Sets display CS low                                           */
/************************************************************************/
void display_cs_low(void) {
	DISPLAY_PORT &= ~(1<<DISPLAY_CS);	
}