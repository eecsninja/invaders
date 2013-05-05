#include <stdint.h>
#include <avr/pgmspace.h>
const uint32_t alien_1_bmp_pal_data32[] PROGMEM = {
	0x0000,0x40003,0x0500,0x0005,0x70106,0x0406,0x80000,0xf0000,
	0x80107,0x0d00,0x0008,0x060a,0x1000d,0xa0208,0xe020c,0x080c,
	0x100000,0x140000,0x1f0000,0x10030e,0x1000,0x1500,0x1800,0x1e00,
	0x171100,0x20012,0x0c13,0x2001b,0x130310,0x130410,0x140411,0x170414,
	0x1a0516,0x1d0518,0x1017,0x131c,0x200000,0x240000,0x370000,0x3a0000,
	0x3d0000,0x22071d,0x2000,0x2700,0x2900,0x2c00,0x3100,0x3900,
	0x3c00,0x20123,0x80020,0xa0026,0x3012c,0x1621,0x1925,0x1c2a,
	0x40132,0x5023c,0x2a0823,0x350a2d,0x380b2f,0x3c0b32,0x2233,0x2638,
	0x293c,0x420000,0x450000,0x4e0000,0x510000,0x550000,0x5c0000,0x610000,
	0x660000,0x690000,0x6d0000,0x710000,0x740000,0x780000,0x7e0000,0x533f00,
	0x400d35,0x460e3b,0x490e3d,0x4100,0x4600,0x4800,0x4d00,0x5000,
	0x5400,0x5b00,0x5d00,0x6100,0x6500,0x6a00,0x6c00,0x7200,
	0x7500,0x7900,0x7d00,0x6a5100,0x6e5400,0x50244,0x6024b,0x60354,
	0x70359,0x8035c,0x2d43,0x3046,0x344b,0x3750,0x3a54,0x3d5a,
	0x80362,0x8036b,0x90472,0xa047c,0x21007e,0x4d0f41,0x501043,0x511044,
	0x541147,0x561148,0x58114a,0x5b124d,0x5d124e,0x611351,0x611452,0x641454,
	0x691558,0x6c155b,0x6e165c,0x70165e,0x721760,0x741761,0x761864,0x791866,
	0x7d1969,0x415e,0x4564,0x4a6b,0x4e71,0x5174,0x557a,0x4c4c4c,
	0x810000,0x850000,0x890000,0x8d0000,0x910000,0x950000,0x990000,0x9c0000,
	0xa00000,0xa40000,0xa90000,0xad0000,0xb10000,0xb60000,0xba0000,0xbd0000,
	0xc10000,0x801a6b,0x821a6d,0x861b71,0x891b72,0x8a1c74,0x8d1c76,0x8e1c78,
	0x911d7a,0x931e7c,0x951e7d,0x8100,0x8500,0x8900,0x8d00,0x9100,
	0x9500,0x9a00,0x9d00,0xa100,0xa600,0xaa00,0xae00,0xb100,
	0xb600,0xb800,0xbd00,0xc100,0xa0484,0xb058b,0xc0593,0xd069c,
	0x220083,0x25008d,0x260093,0x270095,0xd06a4,0xe06ab,0xf06b3,0xf07b8,
	0x1007b7,0x1007bc,0x2a00a1,0x2b00a4,0x5d86,0x5e88,0x638b,0x6590,
	0x6994,0x73a0,0x1007c3,0x1108c4,0x1107c8,0x1108cb,0x1208d3,0x1208dc,
	0x1308e3,0x1509f1,0x991f80,0x9c1f83,0x9d1f84,0x9c2083,0x9e2084,0xa02086,
	0xa12088,0xa5218a,0xa6218c,0xa9228e,0xab2290,0xad2291,0xb02394,0xb22495,
	0xb52498,0xb8259a,0xba259c,0xbc269e,0xbf26a0,0xc026a1,0xc227a4,0xc427a4,
	0xc528a6,0xca28aa,0xcd29ad,0xd02aaf,0xd12ab0,0xd42bb2,0xd62bb4,0xd92cb6,
	0xdc2cb9,0xe12dbe,0xe72ec2,0xe82fc3,0xea2fc4,0xec30c6,0xef30c9,0xffff00,
};
uint8_t* alien_1_bmp_pal_data8 = (uint8_t*) alien_1_bmp_pal_data32;
uint16_t* alien_1_bmp_pal_data16 = (uint16_t*) alien_1_bmp_pal_data32;
const int ALIEN_1_BMP_PAL_DATA_SIZE = 1024;