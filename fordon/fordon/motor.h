#ifndef MOTOR_H
#define MOTOR_H
#include <avr/interrupt.h>
#include <stdint.h>

void motor_init(void);
void motor_right_set_speed(uint8_t speed);
void motor_left_set_speed(uint8_t speed);
void motor_right_set_clockwise(void);
void motor_right_set_cclockwise(void);
void motor_left_set_clockwise(void);
void motor_left_set_cclockwise(void);
void motor_stop(void);
void motor_set_speed_divider(uint8_t div);
uint8_t motor_conv_duty(uint8_t speed);
uint8_t motor_conv_freq(uint8_t speed);
uint8_t motor_get_speed_left(void);
uint8_t motor_get_speed_right(void);
uint8_t motor_get_freq_left(void);
uint8_t motor_get_freq_right(void);
ISR(TIMER2_COMPA_vect);
ISR(INT0_vect);
ISR(INT1_vect);


#endif