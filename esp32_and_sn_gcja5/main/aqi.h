#include <stdio.h>  // for uint8_t

int pm25_to_aqi(int pm25);

int pm10_to_aqi(int pm10);

void parse_uart(uint8_t data[]);