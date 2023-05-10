#ifndef USART_H
#define USART_H

#include <stdio.h>
#include <stdint.h>

void usart_init(void);
void usart_send_byte(uint8_t byte);
uint8_t usart_recv_byte(void);
void usart_send_string(char *string, uint8_t start, uint8_t stop);

#endif