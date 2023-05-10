#ifndef SRF02_H
#define SRF02_H
#include <stdint.h>

void srf02_start_ranging(void);
uint8_t srf02_read_distance_front(void);
uint8_t srf02_read_distance_back(void);

#endif