/*
 * pcf8574a.c
 *
 * Created: 2021-05-10
 * Author : Jakob Fridesjo
 */ 

#include <avr/interrupt.h>
#include "pcf8574a.h"
#include "i2cmaster.h"
#include "defines.h"

// These global variables store the current state of port/pin/register
uint8_t PCF_DDR  = (1<<PCF_PIR) | (1<<PCF_BUTTON) | 
				   (1<<PCF_LEFT_TRIGGER) | (1<<PCF_RIGHT_TRIGGER);
uint8_t PCF_PORT = 0xFF;
uint8_t PCF_PIN  = 0x00;

/************************************************************************/
/* @brief Turns the green led on port expander on                       */
/************************************************************************/
void pcf8574a_led_green_on(void) {
	PCF_PORT &= ~(1<<PCF_LED_GREEN);
	pcf8574a_set_pin_mode();
}

/************************************************************************/
/* @brief Turns the green led on port expander off                      */
/************************************************************************/
void pcf8574a_led_green_off(void) {
	PCF_PORT |= (1<<PCF_LED_GREEN);
	pcf8574a_set_pin_mode();
}

/************************************************************************/
/* @brief Turns the yellow led on port expander on                      */
/************************************************************************/
void pcf8574a_led_yellow_on(void) {
	PCF_PORT &= ~(1<<PCF_LED_YELLOW);
	pcf8574a_set_pin_mode();
}

/************************************************************************/
/* @brief Turns the yellow led on port expander on                      */
/************************************************************************/
void pcf8574a_led_yellow_off(void) {
	PCF_PORT |= (1<<PCF_LED_YELLOW);
	pcf8574a_set_pin_mode();
	
}

/************************************************************************/
/* @brief Sets the pin as high or low                                   */
/************************************************************************/
void pcf8574a_set_pin_mode(void) {
	cli();
	// Start I2C on Port expander address
	i2c_start(PORT_EXPANDER_ADDRESS+I2C_WRITE);
	// Set pin modes and write to Port expander
	uint8_t pin_val = (PCF_PIN & ~PCF_DDR) | PCF_PORT;
	i2c_write(pin_val);
	i2c_stop();
	sei();
}

/************************************************************************/
/* @brief Reads the button/switch and PIR states                        */
/* @return State of port expander pins                                  */
/************************************************************************/
uint8_t pcf8574a_get_state(void) {
	cli();
	uint8_t byte = 0;
	// Start I2C on Port expander address
	i2c_start(PORT_EXPANDER_ADDRESS+I2C_READ);
	// Read from port expander
	byte = i2c_readAck();
	i2c_readNak();
	i2c_stop();
	sei();
	return ~byte & PCF_DDR;
}