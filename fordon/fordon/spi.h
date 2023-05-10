#ifndef SPI_H
#define SPI_H
#include <stdint.h>

void spi_master_init(void);
uint8_t spi_txrx(uint8_t byte);

#endif