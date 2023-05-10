/*
 * utils.c
 *
 * Created: 2021-05-10
 * Author : Jakob Fridesjo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <avr/wdt.h>

#include "utils.h"
#include "display.h"

/************************************************************************/
/* @brief Enables a WDT 1s timeout (8.0Mhz)                             */
/************************************************************************/
void WDT_Prescaler_Change(void) {
	cli();
	wdt_reset();
	// Start timed  sequence
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	// 32K cycles (~0.25 s)
	WDTCSR = (1<<WDIE) | (1<<WDP2);
	sei();
}

/************************************************************************/
/* @brief Prints sensor readings                                        */
/* @dist_f Distance of front sensor in cm                               */
/* @dist_b Distance of back sensor in cm                                */
/* @string PIR sensor status                                            */
/* @string Left trigger status                                          */
/* @string Right trigger status                                         */
/************************************************************************/
void display_sensor_readings(uint8_t dist_f, uint8_t dist_b, 
	uint8_t pir, uint8_t trigg_l, uint8_t trigg_h) {
	display_cursor_home();
	printf("F:%03dcm  B:%03dcm", dist_f, dist_b);
	printf("%01d", trigg_l);
	if (pir) {
		printf("    Motion    ");
	} 
	else {
		printf("  No motion   ");
	}
	printf("%01d", trigg_h);
}