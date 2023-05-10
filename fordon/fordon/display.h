#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>

void display_cs_high(void);
void display_cs_low(void);
void display_init(void);
void display_clear(void);
void display_move_cursor(uint8_t steps);
void display_write_char(uint8_t c);
void display_write_string(char *string, uint8_t start, uint8_t stop);
void display_cursor_home(void);

#endif