#ifndef SUMMER_H
#define SUMMER_H
#include <stdint.h>

void summer_on(void);
void summer_off(void);
void summer_init(uint16_t hz);
void summer_play_note(char *note);
void summer_add_note(char *note);
void summer_play_tune(void);
void summer_clear_tune(void);

#endif