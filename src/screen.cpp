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
#include "cc_core.h"
#include "registers.h"
#include "sprite_registers.h"
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

    void Screen::update() {
#ifndef __AVR__
        SDL_BlitSurface(wave_background, NULL, screen, NULL);
#endif
        flush_blits();
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

            // Initialize each sprite's color key and data offset.
            for (int i = 0; i < num_objects_of_type; ++i) {
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

    void Screen::update_sprite(const GameEntities::GameEntity* object) {
        // TODO: add code here.
    }

}
