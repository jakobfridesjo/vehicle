#ifndef PCF8574A_H
#define PCF8574A_H

#include <stdint.h>

void pcf8574a_led_yellow_on(void);
void pcf8574a_led_yellow_off(void);
void pcf8574a_led_green_on(void);
void pcf8574a_led_green_off(void);
void pcf8574a_set_pin_mode(void);
uint8_t pcf8574a_get_state(void);

#endif