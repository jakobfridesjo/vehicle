/*
 * usart.c
 *
 * Created: 2021-05-10
 * Author : Jakob Fridesjo
 */ 

#include "usart.h"
#include "defines.h"
#include <string.h>

/************************************************************************/
/* @brief Initializes USART                                             */
/************************************************************************/
void usart_init() {
	UBRR0H = 0x00;
	UBRR0L = BAUD;
	// Enable receiver and transmitter, including receiver interrupt
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
	// Set fram format: 8data, 2stop bit
	UCSR0C |= (1<<USBS0) | (3<<UCSZ00);
}

/************************************************************************/
/* @brief Sends a byte using USART                                      */
/************************************************************************/
void usart_send_byte(uint8_t byte) {
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = byte;
}

/************************************************************************/
/* @brief Receives a byte from USART                                    */
/* @return Received byte or 0x00                                        */
/************************************************************************/
uint8_t usart_recv_byte(void) {
	if (UCSR0A & (1<<RXC0)) {
		return UDR0;
	}
	return 0x00;
}

/************************************************************************/
/* @brief Sends a string using USART                                    */
/* @string String to be sent over USART                                 */
/* @start Position to start reading from                                */
/* @stop Position to stop reading when at                               */
/************************************************************************/
void usart_send_string(char *string, uint8_t start, uint8_t stop) {
	for (uint8_t i = start; i < stop; i++) {
		usart_send_byte((uint8_t) string[i]);
	}
}