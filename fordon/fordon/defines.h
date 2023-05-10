#ifndef DEFINES_H
#define DEFINES_H
#include <avr/io.h>

// Define baud for USART
//#define BAUD 25 // Baud = 2400 (1.0Mhz)
//#define BAUD 12 // Baud = 4800 (1.0Mhz)
//#define BAUD 6 // Baud = 9600 (1.0Mhz)
//#define BAUD 0   // Baud = 0.5m (8.0Mhz)
//#define BAUD 1   // Baud = 250k (8.0Mhz)
//#define BAUD 1   // Baud = 230.4k (8.0Mhz)
//#define BAUD 3   // Baud = 115.2k (8.0Mhz)
//#define BAUD 6   // Baud = 76.8k (8.0Mhz)
//#define BAUD 8   // Baud = 57.6k (8.0Mhz)
//#define BAUD 12   // Baud = 38.4k (8.0Mhz)
//#define BAUD 16   // Baud = 28.8k (8.0Mhz)
//#define BAUD 25   // Baud = 19.2k (8.0Mhz)
//#define BAUD 34   // Baud = 14.4k (8.0Mhz)
#define BAUD 51   // Baud = 9600 (8.0Mhz)
//#define BAUD 103  // Baud = 4800 (8.0Mhz)
//#define BAUD 207  // Baud = 2400 (8.0Mhz)

// Define register, port and pins for LED
#define LED_DDR DDRC
#define LED_PORT PORTC
#define LED_PIN_RED  PINC0
#define LED_STRIP PIND4

// Define register, port and pins for display
#define DISPLAY_DDR DDRB
#define DISPLAY_PORT PORTB
#define DISPLAY_PORT_RS PORTC
#define DISPLAY_DDR_RS DDRC
#define DISPLAY_RS PINC1
#define DISPLAY_CS PINB2

// Define register, port and pin for summer
#define SUMMER_DDR DDRB
#define SUMMER_PORT PORTB
#define SUMMER_PIN PINB1
#define SUMMER_COM COM1A0

// Define Motor pins, ports and registers
#define MOTOR_LEFT_DIR_CLOCKWISE PINB6
#define MOTOR_LEFT_DIR_CCLOCKWISE PINB7
#define MOTOR_RIGHT_DIR_CLOCKWISE PIND7
#define MOTOR_RIGHT_DIR_CCLOCKWISE PINB0
#define MOTOR_LEFT_PWM PIND5
#define MOTOR_RIGHT_PWM PIND6
#define MOTOR_LEFT_COM COM0B1
#define MOTOR_RIGHT_COM COM0A1
#define MOTOR_LEFT_ENC PIND3
#define MOTOR_RIGHT_ENC PIND2
#define MOTOR_RIGHT_OCR OCR0A
#define MOTOR_LEFT_OCR OCR0B
#define MOTOR_SPEED_DIVIDER 4
#define MOTOR_FAST_DIV 1
#define MOTOR_SLOW_DIV 3

// Definitions for distance sensors
#define SRF02_FRONT 0xE0
#define SRF02_BACK 0xEA
#define SRF02_CMD 0x00
#define SRF02_UNUSED_REG 0x01
#define SRF02_HIGH_RANGE 0x02
#define SRF02_LOW_RANGE 0x03
#define SRF02_CMD_CM 0x51 

// Definitions for Port expander
#define PORT_EXPANDER_ADDRESS 0x70
#define PCF_LED_YELLOW 0
#define PCF_LED_GREEN 1
#define PCF_RIGHT_TRIGGER 2
#define PCF_LEFT_TRIGGER 3
#define PCF_BUTTON 4
#define PCF_PIR 5

// Other defines
#define ENCODER_TIMER 0
#define MAX_SONG_LENGTH 128
#define BUFSIZE 64
#define MESSAGE_LENGTH 17

// Make code a bit more easily readable with booleans
#define TRUE 1
#define FALSE 0

#endif