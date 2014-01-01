/*
 screen.cpp
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

#include "screen.h"

#include <stdio.h>

#include <DuinoCube.h>

#include "game_entity.h"
#include "printf.h"

// TODO: This should be included from a ChronoCube library file.
#define MAX_NUM_SPRITES      128

#define DEFAULT_COLOR_KEY   0xff

#define SPRITE_LAYER_INDEX     3

extern uint16_t g_vram_offsets[];

namespace {

    // Values of the sprite dimension fields in the sprite control register.
    enum {
        SPRITE_DIMENSION_8,
        SPRITE_DIMENSION_16,
        SPRITE_DIMENSION_32,
        SPRITE_DIMENSION_64,
    };

    // Gets the sprite dimension code for a given dimension that is one of:
    // 8, 16, 32, 64.
    // If it doesn't match any of these, returns the code for 8.
    uint8_t get_sprite_dimension(uint8_t size) {
        switch(size) {
        case 64:
            return SPRITE_DIMENSION_64;
        case 32:
            return SPRITE_DIMENSION_32;
        case 16:
            return SPRITE_DIMENSION_16;
        case 8:
        default:
            return SPRITE_DIMENSION_8;
        }
    }

}

namespace Graphics {
    Screen::Screen() : allocated_vram_size(0) {}

    bool Screen::init() {
        memset(num_sprites_per_type, 0, sizeof(num_sprites_per_type));
        memset(sprite_index_bases, 0, sizeof(sprite_index_bases));

        DC.Core.writeWord(REG_SPRITE_Z, SPRITE_LAYER_INDEX);

        return true;
    }

    void Screen::begin_update() {
        // Wait for the start of vertical blank, at which point it is safe to
        // modify the contents of the video controller.
        while (!(DC.Core.readWord(REG_OUTPUT_STATUS) & (1 << REG_VBLANK)));
    }

    void Screen::update() {
        // Wait for the end of vertical blank.  This is when the drawing
        // actually begins.
        while ((DC.Core.readWord(REG_OUTPUT_STATUS) & (1 << REG_VBLANK)));
    }

    void Screen::allocate_sprites(const int* num_objects_per_type) {
        uint16_t sprite_index = 0;
        for (int type = 0;
             type < NUM_GAME_ENTITY_TYPES && sprite_index < MAX_NUM_SPRITES;
             ++type) {
            // Store the number of sprites and first sprite index for each type.
            int num_objects_of_type = num_objects_per_type[type];
            num_sprites_per_type[type] = num_objects_of_type;
            sprite_index_bases[type] = sprite_index;
            printf_P("Allocated %u sprites starting at %u for object type %d\n",
                     num_objects_of_type, sprite_index, type);

            if (num_objects_of_type == 0)
                continue;

            const GameEntities::GameEntityTypeProperties* properties =
                GameEntities::GameEntity::get_type_property(type);

            uint8_t sprite_w = get_sprite_dimension(properties->sprite_w);
            uint8_t sprite_h = get_sprite_dimension(properties->sprite_h);
            // Initialize each sprite's dimensions, color key, and data offset.
            for (int i = 0; i < num_objects_of_type; ++i) {
                DC.Core.writeWord(SPRITE_REG(i, SPRITE_CTRL_1),
                                  (sprite_w << SPRITE_HSIZE_0) |
                                  (sprite_h << SPRITE_VSIZE_0));
                DC.Core.writeWord(SPRITE_REG(i, SPRITE_COLOR_KEY),
                                  DEFAULT_COLOR_KEY);
                DC.Core.writeWord(SPRITE_REG(i, SPRITE_DATA_OFFSET),
                                  get_image_offset(type));
            }

            sprite_index += num_objects_of_type;
        }
        if (sprite_index == MAX_NUM_SPRITES) {
            printf_P("Attempted to allocate too many sprites: %d\n",
                     sprite_index);
        }
    }

    void Screen::set_palette_data(uint8_t palette, const void* palette_data,
                                  uint16_t size) {
        DC.Core.writeData(PALETTE(palette), palette_data, size);
    }

    void Screen::set_palette_entry(uint8_t palette, uint8_t entry,
                                   uint8_t r, uint8_t g, uint8_t b) {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t padding;
        } palette_entry;
        palette_entry.r = r;
        palette_entry.g = g;
        palette_entry.b = b;
        palette_entry.padding = 0;
        DC.Core.writeData(PALETTE(palette) + sizeof(palette_entry) * entry,
                          &palette_entry, sizeof(palette_entry));
    }

    void Screen::setup_tile_layer(uint8_t layer, bool enabled, uint8_t palette,
                                  uint16_t data_offset, uint16_t color_key) {
        uint16_t tile_ctrl0_value =
            ((enabled ? 1 : 0) << TILE_LAYER_ENABLED) |
            (1 << TILE_ENABLE_NOP) |
            (1 << TILE_ENABLE_TRANSP) |
            (1 << TILE_ENABLE_FLIP) |
            (palette << TILE_PALETTE_START);
        DC.Core.writeWord(TILE_LAYER_REG(layer, TILE_CTRL_0), tile_ctrl0_value);
        DC.Core.writeWord(TILE_LAYER_REG(layer, TILE_DATA_OFFSET), data_offset);
        DC.Core.writeWord(TILE_LAYER_REG(layer, TILE_EMPTY_VALUE), 0);
        DC.Core.writeWord(TILE_LAYER_REG(layer, TILE_COLOR_KEY), color_key);
    }

    void Screen::scroll_tile_layer(uint8_t layer, int16_t x, int16_t y) {
        DC.Core.writeWord(TILE_LAYER_REG(layer, TILE_OFFSET_X), x);
        DC.Core.writeWord(TILE_LAYER_REG(layer, TILE_OFFSET_Y), y);
    }

    void Screen::set_tilemap_data(uint8_t layer, uint8_t x, uint8_t y,
                                  const void* tilemap_data, uint16_t size) {
#define TILEMAP_WIDTH     32
        uint16_t offset = (x + y * TILEMAP_WIDTH) * sizeof(uint16_t);
        DC.Core.writeData(TILEMAP(layer) + offset, tilemap_data, size);
#undef TILEMAP_WIDTH;
    }

    uint16_t Screen::get_image_offset(int type) const {
        return g_vram_offsets[type];
    }

    void Screen::update_sprite(const GameEntities::GameEntity* object) {
        uint8_t type = object->get_type();
        if (num_sprites_per_type[type] == 0)
            return;
        uint8_t index = object->get_index();

        int x = object->get_x();
        int y = object->get_y();

        const GameEntities::GameEntityTypeProperties* properties =
            GameEntities::GameEntity::get_type_property(type);
        uint8_t sprite_w = properties->sprite_w;
        uint8_t sprite_h = properties->sprite_h;
        uint16_t offset = get_image_offset(type) +
                (sprite_w * sprite_h) * object->get_current_image();
        uint16_t sprite_index = sprite_index_bases[type] + index;
        DC.Core.writeWord(SPRITE_REG(sprite_index, SPRITE_CTRL_0),
                          object->is_alive() |
                          (1 << SPRITE_ENABLE_TRANSP));
        DC.Core.writeWord(SPRITE_REG(sprite_index, SPRITE_DATA_OFFSET), offset);
        // TODO: this needs to be faster.
        DC.Core.writeWord(SPRITE_REG(sprite_index, SPRITE_OFFSET_X), x);
        DC.Core.writeWord(SPRITE_REG(sprite_index, SPRITE_OFFSET_Y), y);
    }

}
