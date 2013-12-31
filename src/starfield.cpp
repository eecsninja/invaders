/*
 starfield.cpp
 Classic Invaders

 Copyright (c) 2013, Todd Steinackle, Simon Que
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted
 provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions
 and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the documentation and/or other
 materials provided with the distribution.

 * Neither the name of The No Quarter Arcade (http://www.noquarterarcade.com/)  nor the names of
 its contributors may be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "starfield.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <DuinoCube.h>

#include "screen.h"

#define MAX_LINE_SIZE     64
#define TILEMAP_WIDTH     32
#define TILEMAP_HEIGHT    32

// Generates randomized starfield tiles and tilemap.
void generate_starfield(Graphics::Screen* screen,
                        uint8_t layer, uint8_t palette,
                        uint8_t tile_width, uint8_t tile_height,
                        uint8_t num_tiles, uint16_t num_stars,
                        uint8_t min_brightness, uint8_t max_brightness) {
    if (tile_width > MAX_LINE_SIZE) {
        fprintf(stderr, "Starfield tiles cannot be wider than %u pixels.\n",
                MAX_LINE_SIZE);
        return;
    }
    if (max_brightness < min_brightness) {
        fprintf(stderr, " Min brightness must be lower than max brightness.\n");
        return;
    }

    // Allocate space in VRAM.
    uint16_t vram_base;
    uint16_t tile_size = tile_width * tile_height;
    if (!screen->allocate_vram(tile_size * num_tiles, &vram_base)) {
        fprintf(stderr, "Could not allocate %u bytes in VRAM\n",
                tile_size * num_tiles);
        return;
    }
    DC.Core.writeWord(REG_SYS_CTRL, (1 << REG_SYS_CTRL_VRAM_ACCESS));
    DC.Core.writeWord(REG_MEM_BANK,
                      VRAM_BANK_BEGIN + vram_base / VRAM_BANK_SIZE);

    // Generate the tiles.
    uint16_t offset = 0;
    for (uint8_t i = 0; i < num_tiles; ++i) {
        uint8_t buffer[MAX_LINE_SIZE];
        for (uint8_t y = 0; y < tile_height; ++y, offset += tile_width) {
            memset(buffer, 0, tile_width);
            for (uint8_t x = 0; x < tile_width; ++x) {
                uint16_t rand_num = rand() % (tile_width * tile_height);
                if (rand_num >= num_stars) {
                    continue;
                }
                uint8_t brightness = min_brightness +
                        (uint8_t)rand() % (max_brightness - min_brightness + 1);
                buffer[x] = brightness;
            }
            printf("Writing %d bytes of starfield data to 0x%04x\n",
                   vram_base + offset);
            DC.Core.writeData(vram_base + offset, buffer, tile_width);
        }
    }
    DC.Core.writeWord(REG_SYS_CTRL, (0 << REG_SYS_CTRL_VRAM_ACCESS));

    DC.Core.writeWord(REG_MEM_BANK, TILEMAP_BANK);

    for (uint8_t y = 0; y < TILEMAP_HEIGHT; ++y) {
        uint16_t buffer[TILEMAP_WIDTH];
        for (uint8_t x = 0; x < TILEMAP_WIDTH; ++x)
            buffer[x] = rand() % num_tiles;
        screen->set_tilemap_data(layer, 0, y, buffer, sizeof(buffer));
    }

    // Set up grayscale palette.
    for (uint8_t color = 0; true ; ++color) {
        screen->set_palette_entry(palette, color, color, color, color);
        if (color == 0xff)
            break;
    }

    // Set up and enable the tile layer and tile map.
    screen->setup_tile_layer(layer, true, palette, vram_base, 0);
}
