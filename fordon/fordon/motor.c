/*
 * srf02.c
 *
 * Created: 2021-05-10
 * Author : Jakob Fridesjo
 */

#include "motor.h"
#include "defines.h"
#include "pcf8574a.h"
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t motor_speed_div = 1;
volatile uint8_t motor_left_freq = 0;
volatile uint8_t motor_right_freq = 0;
volatile uint16_t enc_right_count = 0;
volatile uint16_t enc_left_count = 0;
volatile uint8_t enc_right_freq = 0;
volatile uint8_t enc_left_freq = 0;
volatile uint16_t timer2_count0 = 0;
volatile uint16_t timer2_count1 = 0;

void motor_init(void) {
	cli();
	
	// Setup timer0 for both motors
	MOTOR_RIGHT_OCR = 0;
	MOTOR_LEFT_OCR = 0;
	// Setup pins
	TCCR0A |= (1<<MOTOR_LEFT_COM) | (1<<MOTOR_RIGHT_COM);
	// set none-inverting mode with phase correct PWM
	TCCR0A |= (1 << WGM02) | (1 << WGM01) | (1 << WGM00);
	// Set prescaler to 8.
	TCCR0B |= (1<<CS00);
	
	// Enable motor outputs
	DDRD |= (1<<MOTOR_LEFT_PWM) | (1<<MOTOR_RIGHT_PWM);

	// Set motor direction to clockwise
	DDRB |= (1<<MOTOR_LEFT_DIR_CCLOCKWISE);
	DDRD |= (1<<MOTOR_RIGHT_DIR_CCLOCKWISE);
	DDRB |= (1<<MOTOR_LEFT_DIR_CLOCKWISE);
	DDRB |= (1<<MOTOR_RIGHT_DIR_CLOCKWISE);
	motor_left_set_clockwise();
	motor_right_set_clockwise();

	// Setup encoder
	DDRD &= ~((1<<MOTOR_LEFT_ENC) | (1<<MOTOR_RIGHT_ENC));
	sei();	
}

/************************************************************************/
/* @brief Set speed divider for motor                                   */
/************************************************************************/
void motor_set_speed_divider(uint8_t div) {
	// Restore speed
	motor_left_freq *= motor_speed_div;
	motor_right_freq *= motor_speed_div;
	// Apply new divider
	motor_left_freq /= div;
	motor_right_freq /= div;
	motor_speed_div = div;
}

/************************************************************************/
/* @brief Sets the left motor speed                                     */
/* @speed Set the left motor to this speed (0-9)                        */
/************************************************************************/
void motor_left_set_speed(uint8_t speed) {
	// Apply default duty
	MOTOR_LEFT_OCR = motor_conv_duty(speed) / motor_speed_div 
											+ (10 * motor_speed_div) + 5;
	// Apply frequency and divider
	motor_left_freq = motor_conv_freq(speed) / motor_speed_div;
}

/************************************************************************/
/* @brief Sets the right motor speed                                    */
/* @speed Set the right motor to this speed (0-9)                       */
/************************************************************************/
void motor_right_set_speed(uint8_t speed) {
	// Apply default duty
	MOTOR_RIGHT_OCR = motor_conv_duty(speed) / motor_speed_div 
											 + (10 * motor_speed_div) + 5;
	// Apply frequency and divider
	motor_right_freq = motor_conv_freq(speed) / motor_speed_div;
}

/************************************************************************/
/* @brief Converts motor speed to correct duty                          */
/* @speed (0-9)                                                         */
/* @return Converted speed                                              */
/************************************************************************/
uint8_t motor_conv_duty(uint8_t speed) {
	switch (speed) {
		case 0:
			return 0;
		case 1:
			return 80;
		case 2:
			return 80;
		case 3:
			return 80;
		case 4:
			return 80;
		case 5:
			return 80;
		case 6:
			return 100;
		case 7:
			return 120;
		case 8:
			return 140;
		case 9:
			return 160;
	}
	return 0;
}

/************************************************************************/
/* @brief Gets the correct motor frequency                              */
/* @speed (0-9)                                                         */
/* @return Converted speed                                              */
/************************************************************************/
uint8_t motor_conv_freq(uint8_t speed) {
	switch (speed) {
		case 0:
			return 0;
		case 1:
			return 40;
		case 2:
			return 60;
		case 3:
			return 80;
		case 4:
			return 90;
		case 5:
			return 110;
		case 6:
			return 120;
		case 7:
			return 130;
		case 8:
			return 140;
		case 9:
			return 150;
	}
	return 0;
}

/************************************************************************/
/* @brief Set right motor to clockwise turn                             */
/************************************************************************/
void motor_right_set_clockwise(void) {
	PORTB |= (1<<MOTOR_RIGHT_DIR_CCLOCKWISE);
	PORTD &= ~(1<<MOTOR_RIGHT_DIR_CLOCKWISE);
}

/************************************************************************/
/* @brief Set right motor to counter-clockwise turn                     */
/************************************************************************/
void motor_right_set_cclockwise(void) {
	PORTB &= ~(1<<MOTOR_RIGHT_DIR_CCLOCKWISE);
	PORTD |= (1<<MOTOR_RIGHT_DIR_CLOCKWISE);
}

/************************************************************************/
/* @brief Set left motor to clockwise turn                              */
/************************************************************************/
void motor_left_set_clockwise(void) {
	PORTB &= ~(1<<MOTOR_LEFT_DIR_CCLOCKWISE);
	PORTB |= (1<<MOTOR_LEFT_DIR_CLOCKWISE);
}

/************************************************************************/
/* @brief Set left motor to counter-clockwise turn                      */
/************************************************************************/
void motor_left_set_cclockwise(void) {
	PORTB |= (1<<MOTOR_LEFT_DIR_CCLOCKWISE);
	PORTB &= ~(1<<MOTOR_LEFT_DIR_CLOCKWISE);
}

/************************************************************************/
/* @brief Stops the motors                                              */
/************************************************************************/
void motor_stop(void) {
	OCR0A = 0;
	OCR0B = 0;
	motor_left_freq = 0;
	motor_right_freq = 0;
}

/************************************************************************/
/* @brief Calculates frequency of motor encoders                        */
/************************************************************************/
ISR(TIMER2_COMPA_vect){
	timer2_count0++;
	timer2_count1++;
	
	// Handle quick stops
	if (motor_left_freq == 0) {
		MOTOR_LEFT_OCR = 0;
	}
	if (motor_right_freq == 0) {
		MOTOR_RIGHT_OCR = 0;
	}

	// 0.1 second
	if (timer2_count0 >= 10) {
		// Calculate frequency
		enc_left_freq = enc_left_count * 5;
		enc_left_count = 0;
		enc_right_freq = enc_right_count * 5;
		enc_right_count = 0;
		timer2_count0 = 0;
	}
	// Don't adjust duty if frequency is supposed to be 0
	if ( (motor_left_freq == 0) && (motor_right_freq == 0)) {
		return;
	}
	// Adjust motor speed
	if (timer2_count1 >= 3) {
		if (MOTOR_LEFT_OCR < 225) {
			if (enc_left_freq < (motor_left_freq-7)) {
				MOTOR_LEFT_OCR+=2;
			}
		}
		if (MOTOR_LEFT_OCR > 30) {
			if (enc_left_freq > (motor_left_freq+7)) {
				MOTOR_LEFT_OCR-=2;
			}
		}
		if (MOTOR_RIGHT_OCR < 225) {
			if (enc_right_freq < (motor_right_freq-7)) {
				MOTOR_RIGHT_OCR+=2;
			}
		}
		if (MOTOR_RIGHT_OCR > 30) {
			if (enc_right_freq > (motor_right_freq+7)) {
				MOTOR_RIGHT_OCR-=2;
			}
		}
		timer2_count1 = 0;
	}
}

/************************************************************************/
/* @brief Gets right speed from encoder                                 */
/* @return Frequency goal of left motor                                 */
/************************************************************************/
uint8_t motor_get_speed_left(void) {
	return motor_left_freq;
}

/************************************************************************/
/* @brief Gets left speed from encoder                                  */
/* @return Frequency goal of right motor                                */
/************************************************************************/
uint8_t motor_get_speed_right(void) {
	return motor_right_freq;
}

/************************************************************************/
/* @brief Gets right speed from encoder                                 */
/* @return Current frequency of left motor                              */
/************************************************************************/
uint8_t motor_get_freq_left(void) {
	return enc_left_freq;
}

/************************************************************************/
/* @brief Gets left speed from encoder                                  */
/* @return Current frequency of right motor                             */
/************************************************************************/
uint8_t motor_get_freq_right(void) {
	return enc_right_freq;
}

/************************************************************************/
/* @brief For counting INT0 frequency                                   */
/************************************************************************/
ISR(INT0_vect) {
	enc_left_count++;
}

/************************************************************************/
/* @brief For counting INT1 frequency                                   */
/************************************************************************/
ISR(INT1_vect) {
	enc_right_count++;
}