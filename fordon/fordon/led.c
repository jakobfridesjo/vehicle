/*
 * led.c
 *
 * Created: 2021-05-10
 * Author : Jakob Fridesjo
 */ 

#include "led.h"
#include "defines.h"

/************************************************************************/
/* @brief Initializes the LED                                           */
/************************************************************************/
void led_init(void) {
	LED_DDR |= (1<<LED_PIN_RED);
}

/************************************************************************/
/* @brief Turns the red LED on                                          */
/************************************************************************/
void led_red_on(void) {
	LED_PORT |= (1<<LED_PIN_RED);
}

/************************************************************************/
/* @brief Turns the red LED off                                         */
/************************************************************************/
void led_red_off(void) {
	LED_PORT &= ~(1<<LED_PIN_RED);
}