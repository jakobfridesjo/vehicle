/*
 * fordon.c
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
#include "defines.h"
#include "display.h"
#include "spi.h"
#include "summer.h"
#include "utils.h"
#include "usart.h"
#include "i2cmaster.h"
#include "pcf8574a.h"
#include "led.h"
#include "motor.h"
#include "srf02.h"

// This flag enables/disabled heartbeat
volatile uint8_t debug = FALSE;

volatile uint8_t process = TRUE;
volatile uint8_t message[MESSAGE_LENGTH] = {' '};
volatile uint8_t buf_index = 0;
volatile uint8_t heartbeat = 0;
volatile uint8_t stopped = FALSE;
volatile uint8_t stopped_crash = FALSE;
volatile uint8_t stopped_deadman = TRUE;
volatile uint8_t send_pong = FALSE;
volatile uint8_t send_b_command = 0;
volatile uint8_t speed_slow = FALSE;
volatile uint8_t send_stop_ack = FALSE;
volatile uint8_t send_dead_man_ack = FALSE;
volatile uint8_t send_heartbeat_ack = FALSE;

volatile uint8_t buf[BUFSIZE] = {0};

/************************************************************************/
/* @brief Redirect stdout                                               */
/************************************************************************/
static int uartPutChar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uartPutChar,NULL,_FDEV_SETUP_WRITE);
static int uartPutChar(char c, FILE *stream){
	//Send char to uart
	if (c == '\n') {
		uartPutChar('\n', stream);
		} else {
		display_write_char((uint8_t) c);
	}
	return 0;
}

/************************************************************************/
/* @brief Sets a timer for frequency counting interrupt                 */
/************************************************************************/
void init_encoder_timer(void) {
	// Timer2 based frequency counter
	cli();
	TCCR2A = 0;
	TCCR2B = 0;
	// Counter begins at 0
	TCNT2 = 0;
	// ~33ms
	OCR2A = 255;
	// Enable CTC mode
	TCCR2B |= (1<<WGM21);
	// Prescaler 1024
	TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);
	TIMSK2 |= (1<<OCIE2A);
	sei();
}

/************************************************************************/
/* @brief Interrupt vector for USART                                    */
/************************************************************************/
ISR(USART_RX_vect) {
	
	// Get byte from UART
	uint8_t recv = 0;
	recv = usart_recv_byte();
	if (recv != '\n') {
		buf[buf_index] = recv;
		buf_index++;
	}
	
	if ((buf[buf_index - 1] == '.')) {
		
	char temp[4];
	buf_index = 0;
	
	// Heartbeat
	switch (buf[0]) {
		case 'h':
			wdt_reset();
			heartbeat = 0;
			break;
		// Motor control
		case 'm': 
			if (!stopped && !stopped_deadman && (heartbeat < 12)) {
				if (buf[2] == '1') {
					motor_left_set_cclockwise();
				} else {
					motor_left_set_clockwise();
				}
				if (buf[5] == '1') {
					motor_right_set_cclockwise();
				} else {
					motor_right_set_clockwise();
				}
				// Convert from ascii
				motor_left_set_speed(buf[3]-48);
				motor_right_set_speed(buf[6]-48);
			} else {
				motor_stop();
			}
			break;
		// Messages
		case 't':
			message[0] = '1';
			for (uint8_t i = 1; i < MESSAGE_LENGTH; i++) {
				message[i] = buf[i+1];
			}
			break;
		// Emergency stop
		case 'n':
			motor_stop();
			led_red_on();
			stopped = TRUE;
			if (buf[1] == '.') {
				break;
			}
			message[0] = '1';
			for (uint8_t i = 1; i < MESSAGE_LENGTH; i++) {
				message[i] = buf[i+1];
			}
			send_stop_ack = TRUE;
			break;
		// Emergency stop unlock
		case 'k':
			stopped = FALSE;
			led_red_off();
			break;
		// Handle slow/fast mode
		case 'g':
			if (buf[2] == '0') {
				pcf8574a_led_yellow_off();
				motor_set_speed_divider(MOTOR_FAST_DIV);
				speed_slow = FALSE;
			} else {
				pcf8574a_led_yellow_on();
				motor_set_speed_divider(MOTOR_SLOW_DIV);
				speed_slow = TRUE;
			}
			break;
		// Handle buzzer
		case 'b':
			if ((buf[2] == '3') && (buf[4] == '0')) {
				summer_off();
			} 
			else if ((buf[2] == '3')) {
				summer_on();
			} 
			else if ((buf[2] == '4') && (buf[4] == '0')) {
				send_b_command = 1;
			} 
			else if ((buf[2] == '4')) {
				send_b_command = 2;
			}
			break;
		// Handle dead man stop/run
		case 'd':
			if ((buf[2] == '3') && (buf[4] == '1')) {
				pcf8574a_led_green_on();
				stopped_deadman = FALSE;
			} 
			else if ((buf[2] == '3') && (buf[4] == '0')) {
				motor_stop();
				pcf8574a_led_green_off();
				stopped_deadman = TRUE;
			}
			else if ((buf[2] == '4') && (buf[4] == '1')) {
				send_dead_man_ack = TRUE;
			}
			break;
		// Save notes to note buffer
		case 'p':
			temp[0] = buf[2];
			temp[1] = buf[3];
			temp[2] = buf[4];
			temp[3] = buf[5];
			summer_add_note(temp);
			break;
		// Respond ping on pong
		case 'P':
			if (strncmp((char*)buf, "PING.", 5) == 0) {
				send_pong = TRUE;
			}
			break;
		}
	}
}

/************************************************************************/
/* @brief Interrupt vector for heartbeat                                */
/************************************************************************/
ISR(WDT_vect) {
	// Let main loop events process
	process = TRUE;
	// Heartbeat ~3 seconds
	if (heartbeat == 12) {
		send_heartbeat_ack = TRUE;
		heartbeat++;
	} 
	else if (heartbeat >= 12) {
		motor_stop();
	}
	else if (!debug) {
		heartbeat++;
	}
}

/************************************************************************/
/* @brief Enable INT0/INT1 interrupts                                   */
/************************************************************************/
void init_external_interrupts(void) {
	// Set pull-up and input on INT0 and INT1
	DDRD &= ~(1<<DDD2);
	PORTD |= (1<<PORTD2);
	DDRD &= ~(1<<DDD3);
	PORTD |= (1<<PORTD3);
	// Enable interrupts
	EICRA |= (1<<ISC11) | (1<<ISC10) | (1<<ISC01);
	EIMSK |= (1<<INT0) | (1<<INT1);
}

/************************************************************************/
/* @brief Initializes all hardware                                      */
/************************************************************************/
void init(void) {
	// Disable interrupts
	cli();
	// Redirect stdout so printf prints to USART
	stdout = &mystdout;
	// For display
	spi_master_init();
	display_init();
	// For USART/MQTT
	usart_init();
	// For LEDS
	led_init();
	// For Summer
	summer_init(16197);
	// For IO-expander, distance sensor and further things.
	i2c_init();
	// Let distance sensor calibrate itself.
	_delay_ms(1000);
	pcf8574a_led_green_off();
	// Initialize motor control
	motor_init();
	// Initialize watchdog for heartbeat functionality
	WDT_Prescaler_Change();
	// Init external interrupts and timer2
	init_encoder_timer();
	init_external_interrupts();
	// Enable interrupts and reset watchdog
	sei();
	wdt_reset();
}

int main(void) {
	// Initialize hw/sw
    init();
	
	// I'm ready, logger sends notes to my note buffer
	usart_send_string("v,3.\n", 0, 5);
	
	// For getting sensor data
	uint8_t byte = 0;
	uint8_t srf02_front = 0;
	uint8_t srf02_back = 0;
	
	// For sending sensor values
	char sensors[18];
	char sensors_prev[18];
	
	// For tracking state of button, switches and crash detection.
	uint8_t lock_button = FALSE;
	uint8_t lock_left_trigger = FALSE;
	uint8_t lock_right_trigger = FALSE;
	uint8_t lock_summer = FALSE;
	uint8_t send_crash_front = FALSE;
	uint8_t send_crash_back = FALSE;

	while (TRUE) {
		// Handle button presses
		byte = pcf8574a_get_state();
		
		// Button on port expander
		if ((byte & (1<<PCF_BUTTON)) && !lock_button) {
			usart_send_string("b,4,1.\n", 0, 7);
			lock_button = TRUE;
		} 
		else if (!(byte & (1<<PCF_BUTTON)) && lock_button) {
			lock_button = FALSE;
			usart_send_string("b,4,0.\n", 0, 7);
		}
		
		// Left crash trigger on port expander
		if ((byte & (1<<PCF_LEFT_TRIGGER)) && !lock_left_trigger) {
			motor_stop();
			usart_send_string("f,0.\n", 0, 5);
			lock_left_trigger = TRUE;
		} 
		else if (!(byte & (1<<PCF_LEFT_TRIGGER)) && lock_left_trigger) {
			lock_left_trigger = FALSE;
		}
		
		// Right crash trigger on port expander
		if ((byte & (1<<PCF_RIGHT_TRIGGER)) && !lock_right_trigger) {
			motor_stop();
			usart_send_string("f,1.\n", 0, 5);
			lock_right_trigger = TRUE;
		} 
		else if (!(byte & (1<<PCF_RIGHT_TRIGGER)) && lock_right_trigger) {
			lock_right_trigger = FALSE;
		}
		
		// Emergency stop ACK
		if (send_stop_ack) {
			send_stop_ack = FALSE;
			usart_send_string("f,2.\n", 0, 5);
		}
		
		// Dead man stop ACK
		if (send_dead_man_ack) {
			send_dead_man_ack = FALSE;
			usart_send_string("f,3.\n", 0, 5);
		}
		
		// No heartbeat stop ACK
		if (send_heartbeat_ack) {
			send_heartbeat_ack = FALSE;
			usart_send_string("f,4.\n", 0, 5);
		}
		
		// Get distance sensor readings
		srf02_start_ranging();
		// Disable interrupts and get distance
		cli();
		srf02_front = srf02_read_distance_front();
		srf02_back = srf02_read_distance_back();
		
		// Slow down when approaching wall
		if (((srf02_front <= 22) || (srf02_front <= 22)) && !speed_slow) {
			motor_set_speed_divider(MOTOR_SLOW_DIV);
		}
		else if (((srf02_front > 22) && (srf02_front > 22)) && !speed_slow){
			motor_set_speed_divider(MOTOR_FAST_DIV);
		}
		
		// Crash detection
		if (((srf02_front <= 17) || (srf02_back <= 17)) && !stopped_crash) {
			motor_stop();
			stopped_crash = TRUE;
			stopped_deadman = TRUE;
		} 
		else if ((srf02_front > 20) && (srf02_back > 20)) {
			stopped_crash = FALSE;
		}
		
		// Enable interrupts
		sei();

		// Display messages on display
		if (message[0] == '1') {
			message[0] = '0';
			display_clear();
			display_move_cursor(32);
			for (uint8_t i = 1; i < MESSAGE_LENGTH; i++) {
				if (message[i] == '.') {
					break;
				}
				display_write_char(message[i]);
			}
		}
		
		// ping... pong!
		if (send_pong) {
			usart_send_string("PONG.\n", 0, 6);
			send_pong = FALSE;
		}
		
		// Check if crashing, send message
		if ((srf02_front <= 15) && !send_crash_front) {
			usart_send_string("f,5.\n", 0, 5);
			send_crash_front = TRUE;
		}
		else if ((srf02_front >= 20)) {
			send_crash_front = FALSE;
		}
		
		// Check if crashing, send message
		if ((srf02_back <= 15) && !send_crash_back) {
			usart_send_string("f,5.\n", 0, 5);
			send_crash_back = TRUE;
		}
		else if ((srf02_back >= 20)) {
			send_crash_back = FALSE;
		}
		
		// Process sensor data
		if (process) {
			process = FALSE;
				
			// Send all sensor values
			sprintf(sensors, "s,%03d,%03d,%01d,%01d,%01d.\n",
			srf02_back,srf02_front,
			((~byte & (1<<PCF_PIR))>>PCF_PIR),
			((byte & (1<<PCF_LEFT_TRIGGER))>>PCF_LEFT_TRIGGER),
			((byte & (1<<PCF_RIGHT_TRIGGER))>>PCF_RIGHT_TRIGGER));
			
			// Handle sensor value sending, don't send if same values as
			// previous sent values
			if (strcmp(sensors, sensors_prev) != 0) {
				usart_send_string(sensors, 0, 17);
				strcpy(sensors_prev, sensors);
			}
				
			// Display sensor readings
			display_sensor_readings(
				srf02_back,
				srf02_front,
				((~byte & (1<<PCF_PIR))>>PCF_PIR), 
				((byte & (1<<PCF_LEFT_TRIGGER))>>PCF_LEFT_TRIGGER),
				((byte & (1<<PCF_RIGHT_TRIGGER))>>PCF_RIGHT_TRIGGER));
			
			// Handle summer on motion detected
			if ((~byte & (1<<PCF_PIR)) && !lock_summer) {
				summer_play_tune();
				lock_summer = TRUE;
			}
			else if (!(~byte & (1<<PCF_PIR))) {
				lock_summer = FALSE;
			}
		}
	}
}