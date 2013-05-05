#include <stdint.h>
#include <avr/pgmspace.h>
const uint32_t shot_bmp_raw_data32[] PROGMEM = {
	0xffff4fff,0xffffffff,0xff4f644f,0xffffffff,0xff4f644f,0xffffffff,0xff4f644f,0xffffffff,
	0xff4f644f,0xffffffff,0xff4f634f,0xffffffff,0xffff4fff,0xffffffff,0xffffffff,0xffffffff,
};
uint8_t* shot_bmp_raw_data8 = (uint8_t*) shot_bmp_raw_data32;
uint16_t* shot_bmp_raw_data16 = (uint16_t*) shot_bmp_raw_data32;
const int SHOT_BMP_RAW_DATA_SIZE = 64;
