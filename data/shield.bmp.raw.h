#include <stdint.h>
#include <avr/pgmspace.h>
const uint32_t shield_bmp_raw_data32[] PROGMEM = {
	0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,
	0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,0x8f8f8f8f,
};
uint8_t* shield_bmp_raw_data8 = (uint8_t*) shield_bmp_raw_data32;
uint16_t* shield_bmp_raw_data16 = (uint16_t*) shield_bmp_raw_data32;
const int SHIELD_BMP_RAW_DATA_SIZE = 64;
