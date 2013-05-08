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

#ifdef __AVR__
#include <avr/pgmspace.h>

#include "cc_core.h"
#include "registers.h"
#include "sprite_registers.h"
#include "tile_registers.h"
#endif

#include "game_entity.h"
#include "images.h"

// TODO: This should be included from a ChronoCube library file.
#define MAX_NUM_SPRITES      128

#define DEFAULT_COLOR_KEY   0xff

namespace Graphics {
    Screen::Screen() : num_blits(0),
                       image_lib(NULL) {}

    bool Screen::init() {
        memset(num_sprites_per_type, 0, sizeof(num_sprites_per_type));
        memset(sprite_index_bases, 0, sizeof(sprite_index_bases));
#ifdef __AVR__
        CC_Init();
        printf("ChronoCube initialization complete.\n");
#else
        int flags = SDL_SWSURFACE;
        screen = SDL_SetVideoMode(screen_w, screen_h, 16, flags);
        if (screen == NULL) {
            fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
            return false;
        }

        clip.x = 0; clip.y = 50; clip.w = screen_w; clip.h = 515;
        SDL_SetClipRect(screen, &clip);
#endif  // defined(__AVR__)
        return true;
    }

    void Screen::set_image_lib(Images* images) {
        image_lib = images;

#ifndef __AVR__
        wave_background = images->get_image("wave_background.png");
        background = images->get_image("background.png");
        ui_header = images->get_image("ui_header.png");
        ui_points = images->get_image("ui_points.png");
#endif
    }

    void Screen::schedule_blit(const GameEntities::GameEntity* object) {
#ifdef __AVR__
        update_sprite(object);
#else
        if (num_blits >= max_updates) {
            fprintf(stderr, "Exceeded max number of blits (%d).\n", max_updates);
            return;
        }

        blit* update = &blits[num_blits++];
        update->type = object->get_type();
        update->image_index = object->get_current_image();
        update->x = object->get_x();
        update->y = object->get_y();
#endif  // defined (__AVR__)
    }

    void Screen::flush_blits() {
#ifndef __AVR__
        for (int i = 0; i < num_blits && image_lib; ++i) {
            SDL_Surface* image =
                image_lib->get_image(blits[i].type, blits[i].image_index);
            if (image) {
                SDL_Rect dst;
                dst.x = blits[i].x;
                dst.y = blits[i].y;
                SDL_BlitSurface(image, NULL, screen, &dst);
            }
        }
        num_blits = 0;
        SDL_UpdateRect(screen, clip.x, clip.y, clip.w, clip.h);
#endif  // !defined (__AVR__)
    }

    void Screen::begin_update() {
#ifdef __AVR__
        // Wait for the start of vertical blank, at which point it is safe to
        // modify the contents of the video controller.
        while(!(CC_GetRegister(CC_REG_OUTPUT_STATUS) & (1 << CC_REG_VBLANK)));
#endif
    }

    void Screen::update() {
#ifdef __AVR__
        // Wait for the end of vertical blank.  This is when the drawing
        // actually begins.
        while((CC_GetRegister(CC_REG_OUTPUT_STATUS) & (1 << CC_REG_VBLANK)));
#else
        SDL_BlitSurface(wave_background, NULL, screen, NULL);
        flush_blits();
#endif
    }

    void Screen::allocate_sprites(const int* num_objects_per_type) {
#ifdef __AVR__
        uint16_t sprite_index = 0;
        for (int type = 0;
             type < NUM_GAME_ENTITY_TYPES && sprite_index < MAX_NUM_SPRITES;
             ++type) {
            // Store the number of sprites and first sprite index for each type.
            int num_objects_of_type = num_objects_per_type[type];
            num_sprites_per_type[type] = num_objects_of_type;
            sprite_index_bases[type] = sprite_index;
            printf("Allocated %u sprites starting at %u for object type %d\n",
                   num_objects_of_type, sprite_index, type);

            if (num_objects_of_type == 0)
                continue;

            const GameEntities::GameEntityTypeProperties* properties =
                GameEntities::GameEntity::get_type_property(type);

            // Initialize each sprite's dimensions, color key, and data offset.
            for (int i = 0; i < num_objects_of_type; ++i) {
                CC_Sprite_SetRegister(sprite_index + i, SPRITE_CTRL1,
                                      (properties->sprite_w << SPRITE_HSIZE_0) |
                                      (properties->sprite_h << SPRITE_VSIZE_0));
                CC_Sprite_SetRegister(sprite_index + i, SPRITE_COLOR_KEY,
                                      DEFAULT_COLOR_KEY);
                CC_Sprite_SetRegister(sprite_index + i, SPRITE_DATA_OFFSET,
                                      image_lib->get_image_offset(type, 0));
            }

            sprite_index += num_objects_of_type;
        }
        if (sprite_index == MAX_NUM_SPRITES) {
            fprintf(stderr, "Attempted to allocate too many sprites: %d\n",
                    sprite_index);
        }
#endif  // defined (__AVR__)
    }

    void Screen::set_palette_data(const void* palette_data, uint16_t size) {
#ifdef __AVR__
#define BUFFER_SIZE       32
        uint32_t buffer[BUFFER_SIZE];
        const uint32_t* data32 = (const uint32_t*) palette_data;
        uint16_t size_copied = 0;
        uint16_t offset = 0;
        while (size_copied + offset * sizeof(uint32_t) < size) {
            buffer[offset] =
                pgm_read_dword_far(data32 + size_copied / sizeof(uint32_t) +
                                   offset);
            ++offset;
            // If the buffer fills up, copy it to VRAM.
            if (offset == BUFFER_SIZE) {
                CC_SetPaletteData(buffer, 0, size_copied, sizeof(buffer));
                size_copied += sizeof(buffer);
                offset = 0;
            }
        }
#undef BUFFER_SIZE
#endif  // defined (__AVR__)
    }

    void Screen::setup_tile_layer(uint8_t layer, bool enabled,
                                  uint16_t data_offset) {
#ifdef __AVR__
        uint16_t tile_ctrl0_value =
            ((enabled ? 1 : 0) << TILE_LAYER_ENABLED) |
            (1 << TILE_ENABLE_NOP) |
            (1 << TILE_ENABLE_TRANSP) |
            (1 << TILE_ENABLE_FLIP);
        CC_TileLayer_SetRegister(layer, TILE_CTRL0, tile_ctrl0_value);
        CC_TileLayer_SetRegister(layer, TILE_DATA_OFFSET, data_offset);
        CC_TileLayer_SetRegister(layer, TILE_NOP_VALUE, 0);
        CC_TileLayer_SetRegister(layer, TILE_COLOR_KEY, DEFAULT_COLOR_KEY);
#endif  // defined (__AVR__)
    }

    void Screen::scroll_tile_layer(uint8_t layer, int16_t x, int16_t y) {
#ifdef __AVR__
        CC_TileLayer_SetRegister(layer, TILE_OFFSET_X, x);
        CC_TileLayer_SetRegister(layer, TILE_OFFSET_Y, y);
#endif  // defined (__AVR__)
    }

    void Screen::set_tilemap_data(uint8_t layer, uint8_t x, uint8_t y,
                                  const void* tilemap_data, uint16_t size) {
#ifdef __AVR__
#define TILEMAP_WIDTH     32
        uint16_t offset = (x + y * TILEMAP_WIDTH) * sizeof(uint16_t);
        CC_TileLayer_SetData(tilemap_data, layer, offset, size);
#undef TILEMAP_WIDTH;
#endif // defined (__AVR__)
    }

    void Screen::update_sprite(const GameEntities::GameEntity* object) {
#ifdef __AVR__
        uint8_t type = object->get_type();
        if (num_sprites_per_type[type] == 0)
            return;
        uint8_t index = object->get_index();

        uint8_t image_index = object->get_current_image();
        int x = object->get_x();
        int y = object->get_y();

        uint16_t offset = image_lib->get_image_offset(type, image_index);
        uint16_t sprite_index = sprite_index_bases[type] + index;
        CC_Sprite_SetRegister(sprite_index, SPRITE_CTRL0,
                              object->is_alive() |
                              (1 << SPRITE_ENABLE_TRANSP));
        CC_Sprite_SetRegister(sprite_index, SPRITE_OFFSET_X, x);
        CC_Sprite_SetRegister(sprite_index, SPRITE_OFFSET_Y, y);
#endif  // defined (__AVR__)
    }

}
