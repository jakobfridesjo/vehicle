/*
 * summer.c
 *
 * Created: 2021-05-10
 * Author : Jakob Fridesjo
 */ 

#include "summer.h"
#include "defines.h"
#include "led.h"
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

// Array that stores notes
char note_buf[MAX_SONG_LENGTH][4];
uint8_t INDEX = 0;

/************************************************************************/
/* @brief Initializes the summer                                        */
/************************************************************************/
void summer_init(uint16_t hz) {
	summer_clear_tune();

	// Set output
	SUMMER_DDR |= (1<<SUMMER_PIN);
	// Setup timer
	// Set bits and set prescaler to 64
	TCCR1B |= (1<<WGM12) | (1<<WGM10) | (1<<CS10);
	// Set OCR0B register frequency
	OCR1A = hz;
	// Set bits COM0B0 and WGM01 in TCCR0A register
	TCCR1A |= (1<<SUMMER_COM);
	// Disable speaker output
	SUMMER_DDR &= ~(1<<SUMMER_PIN);

}

/************************************************************************/
/* @brief Sets OCR1A value for note                                     */
/************************************************************************/
void summer_play_note(char *note) {
	if (!strncmp(note, "c30", 3)) {
		OCR1A = 61155;
	} 
	else if (!strncmp(note, "c3s", 3)) {
		OCR1A = 57722;
	} 
	else if (!strncmp(note, "d30", 3)) {
		OCR1A = 54482;
	} 
	else if (!strncmp(note, "d3s", 3)) {
		OCR1A = 51424;
	} 
	else if (!strncmp(note, "e30", 3)) {
		OCR1A = 48538;
	} 
	else if (!strncmp(note, "f30", 3)) {
		OCR1A = 45814;
	} 
	else if (!strncmp(note, "f3s", 3)) {
		OCR1A = 43242;
	} 
	else if (!strncmp(note, "g30", 3)) {
		OCR1A = 40815;
	} 
	else if (!strncmp(note, "g3s", 3)) {
		OCR1A = 38524;
	} 
	else if (!strncmp(note, "a30", 3)) {
		OCR1A = 36362;
	} 
	else if (!strncmp(note, "a3s", 3)) {
		OCR1A = 34321;
	} 
	else if (!strncmp(note, "b30", 3)) {
		OCR1A = 32395;
	} 
	else if (!strncmp(note, "c40", 3)) {
		OCR1A = 30577;
	} 
	else if (!strncmp(note, "c4s", 3)) {
		OCR1A = 28860;
	} 
	else if (!strncmp(note, "d40", 3)) {
		OCR1A = 27240;
	} 
	else if (!strncmp(note, "d4s", 3)) {
		OCR1A = 25711;
	} 
	else if (!strncmp(note, "e40", 3)) {
		OCR1A = 24268;
	} 
	else if (!strncmp(note, "f40", 3)) {
		OCR1A = 22906;
	} 
	else if (!strncmp(note, "f4s", 3)) {
		OCR1A = 21620;
	} 
	else if (!strncmp(note, "g40", 3)) {
		OCR1A = 20407;
	} 
	else if (!strncmp(note, "g4s", 3)) {
		OCR1A = 19261;
	} 
	else if (!strncmp(note, "a40", 3)) {
		OCR1A = 18180;	
	} 
	else if (!strncmp(note, "a4s", 3)) {
		OCR1A = 17160;
	} 
	else if (!strncmp(note, "b40", 3)) {
		OCR1A = 16197;
	} 
	else if (!strncmp(note, "c50", 3)) {
		OCR1A = 15288;
	} 
	else if (!strncmp(note, "c5s", 3)) {
		OCR1A = 14429;
	} 
	else if (!strncmp(note, "d50", 3)) {
		OCR1A = 13619;
	} 
	else if (!strncmp(note, "d5s", 3)) {
		OCR1A = 12855;
	} 
	else if (!strncmp(note, "e50", 3)) {
		OCR1A = 12133;
	} 
	else if (!strncmp(note, "f50", 3)) {
		OCR1A = 11452;
	} 
	else if (!strncmp(note, "f5s", 3)) {
		OCR1A = 10809;
	} 
	else if (!strncmp(note, "g50", 3)) {
		OCR1A = 10203;
	} 
	else if (!strncmp(note, "g5s", 3)) {
		OCR1A = 9630;
	} 
	else if (!strncmp(note, "a50", 3)) {
		OCR1A = 9089;
	} 
	else if (!strncmp(note, "a5s", 3)) {
		OCR1A = 8579;
	} 
	else if (!strncmp(note, "b50", 3)) {
		OCR1A = 8098;
	} 
	else if (!strncmp(note, "c60", 3)) {
		OCR1A = 7643;
	} 
	else if (!strncmp(note, "c6s", 3)) {
		OCR1A = 7214;
	} 
	else if (!strncmp(note, "d60", 3)) {
		OCR1A = 6809;
	} 
	else if (!strncmp(note, "d6s", 3)) {
		OCR1A = 6427;
	} 
	else if (!strncmp(note, "e60", 3)) {
		OCR1A = 6066;
	} 
	else if (!strncmp(note, "f60", 3)) {
		OCR1A = 5725;
	} 
	else if (!strncmp(note, "f6s", 3)) {
		OCR1A = 5404;
	} 
	else if (!strncmp(note, "g60", 3)) {
		OCR1A = 5101;
	} 
	else if (!strncmp(note, "g6s", 3)) {
		OCR1A = 4814;
	} 
	else if (!strncmp(note, "a60", 3)) {
		OCR1A = 4544;
	} 
	else if (!strncmp(note, "a6s", 3)) {
		OCR1A = 4289;
	} 
	else if (!strncmp(note, "b60", 3)) {
		OCR1A = 4048;
	}
	
	// Check if delay
	if (note[0] != '0') {
		summer_on();
	}
	switch (note[3]) {
		case '1':
			_delay_ms(1000);
			break;
		case '2':
			_delay_ms(500);
			break;
		case '4':
			_delay_ms(250);
			break;
		case '8':
			_delay_ms(125);
			break;
	}
	summer_off();
	_delay_ms(50);
}

/************************************************************************/
/* @brief Adds a note to the note buffer                                */
/************************************************************************/
void summer_add_note(char *note) {
	strncpy(note_buf[INDEX], note, 4);	
	INDEX++;
}

/************************************************************************/
/* @brief Turns the Summer on                                           */
/************************************************************************/
void summer_on(void) {
	SUMMER_DDR |= (1<<SUMMER_PIN);
}

/************************************************************************/
/* @brief Turns the Summer off                                          */
/************************************************************************/
void summer_off(void) {
	SUMMER_DDR &= ~(1<<SUMMER_PIN);
}

/************************************************************************/
/* @brief Plays the song from note buffer                              */
/************************************************************************/
void summer_play_tune(void) {
	for (uint8_t i = 0; i < MAX_SONG_LENGTH; i++) {
		if (note_buf[i][0] == '-') {
			break;
		}
		summer_play_note(note_buf[i]);
	}
	summer_off();
}

/************************************************************************/
/* @brief Clears the song from note buffer                              */
/************************************************************************/
void summer_clear_tune(void) {
	for (uint8_t i = 0; i < MAX_SONG_LENGTH; i++) {
		note_buf[i][0] = '-';
	}
}