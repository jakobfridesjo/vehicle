#ifndef UTILS_H
#define UTILS_H

void WDT_Prescaler_Change(void);
void display_sensor_readings(uint8_t dist_f, uint8_t dist_b, 
	uint8_t pir, uint8_t trigg_l, uint8_t trigg_h);

#endif