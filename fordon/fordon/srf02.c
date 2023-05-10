/*
 * srf02.c
 *
 * Created: 2021-05-14
 * Author : Jakob Fridesjo
 */

#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "srf02.h"
#include "defines.h"
#include "i2cmaster.h"


/************************************************************************/
/* @brief Initializes the sensors and starts ranging on both            */
/************************************************************************/
void srf02_start_ranging(void) {
		cli();
		// Front sensor initialization
		i2c_start(SRF02_FRONT + I2C_WRITE);
		
		// Write to cmd reg and write cmd 0x51 to get distance in cm.
		i2c_write(SRF02_CMD);
		i2c_write(SRF02_CMD_CM);
		// Stop i2c bus
		i2c_stop();

		// Back sensor initialization
		i2c_start(SRF02_BACK + I2C_WRITE);
		
		// Write to cmd reg and write cmd 0x51 to get distance in cm.
		i2c_write(SRF02_CMD);
		i2c_write(SRF02_CMD_CM);
		// Stop i2c bus
		i2c_stop();
		sei();
		// Wait for ranging to complete
		_delay_ms(70);
}

/************************************************************************/
/* @brief Reads the front distance from SRF02 sonic distance sensor     */
/* @return The front distance in cm, 0-255                              */
/************************************************************************/
uint8_t srf02_read_distance_front(void) {
	uint8_t byte_low = 0x00;
	
	// Set registers to read and wait for completed ranging
	i2c_start(SRF02_FRONT + I2C_WRITE);
	i2c_write(SRF02_LOW_RANGE);
	
	// Read from registers
	i2c_rep_start(SRF02_FRONT + I2C_READ);
	
	byte_low = i2c_readAck();
	i2c_readNak();
	i2c_stop();
	// Counter underflows
	if (byte_low < 12) {
		byte_low = 0;
	} 
	else {
		byte_low -= 12;
	}
	return byte_low;
}

/************************************************************************/
/* @brief Reads the back distance from SRF02 sonic distance sensor      */
/* @return The back distance in cm, 0-255                               */
/************************************************************************/
uint8_t srf02_read_distance_back(void) {
	uint8_t byte_low = 0x00;
	
	// Set registers to read and poll for completed ranging
	i2c_start(SRF02_BACK + I2C_WRITE);
	i2c_write(SRF02_LOW_RANGE);
	
	// Read from registers
	i2c_rep_start(SRF02_BACK + I2C_READ);
	
	byte_low = i2c_readAck();
	i2c_readNak();
	i2c_stop();
	// Counter underflows
	if (byte_low < 12) {
		byte_low = 0;
	} 
	else {
		byte_low -= 12;
	}
	return byte_low;
}