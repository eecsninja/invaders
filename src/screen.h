/*
 screen.h
 Classic Invaders

 Copyright (c) 2010, Todd Steinackle
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

#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#ifndef __AVR__
#include <SDL/SDL.h>
#endif

#include "game_entity_types.h"

#ifdef __AVR__
    #define screen_w      320
    #define screen_h      240
    #define side_padding    2
#else
    #define screen_w      800
    #define screen_h      600
    #define side_padding    5
#endif

#define max_updates   360
#define SCREEN_TILE_SIZE           16

namespace GameEntities {
class GameEntity;
}  // namespace GameEntities

namespace Graphics {

    class Images;

    class Screen {
    private:
        // A list of scheduled blits.
#ifndef __AVR__
        struct blit {
            int type;
            int image_index;
            int x;
            int y;
        } blits[max_updates];
#endif
        int num_blits;
        Images* image_lib;

#ifndef __AVR__
        SDL_Surface *screen;       // Video screen
        // Background/UI images
        SDL_Surface *background, *wave_background, *ui_header, *ui_points;

        SDL_Rect clip;   // Clipping rectangle.
#endif

        // Each entry in the array is the starting sprite index for each type of
        // game entity.
        // e.g. for the k-th object of type=N, the sprite index is:
        //        |sprite_index_bases[type] + k|.
        uint8_t sprite_index_bases[NUM_GAME_ENTITY_TYPES];
        // How many sprites are allocated for each type.
        uint8_t num_sprites_per_type[NUM_GAME_ENTITY_TYPES];

        // For VRAM allocation.
        uint32_t allocated_vram_size;

    public:
        Screen();

        // Initialize video screen.  Returns true on success.
        bool init();

        // Returns true if tile layer capability is available.
        bool tile_layers_supported();

        // Set the operating video mode.  Returns true on success.
        bool set_video_mode(bool fullscreen);

        // Provide a pointer to an image library.
        void set_image_lib(Images* images);

        // Add a blit task to the blit queue.
        void schedule_blit(const GameEntities::GameEntity* object);

        // Perform all queued blits and reset the blit counter.
        void flush_blits();

        // Wait for the screen to be ready for updates.
        void begin_update();

        // Redraw the entire screen, including all the scheduled blits.
        void update();

        // Used to initialize the sprite allocation table based on how many
        // of each type of object will be drawn.
        void allocate_sprites(const int* num_objects_per_type);

        // Attempts to allocate VRAM.  Returns true on success.  Returns the
        // allocated address in |*vram_offset|.
        bool allocate_vram(uint16_t size, uint16_t* vram_offset);

        // Loads palette data.
        // TODO: support multiple palettes.
        void set_palette_data(const void* palette_data, uint16_t size);

        // Initializes a tile layer.
        void setup_tile_layer(uint8_t layer, bool enabled, int type);

        // Sets the scroll offset of a tile layer.
        void scroll_tile_layer(uint8_t layer, int16_t x, int16_t y);

        // Copies tilemap data into tilemap memory, indicated by |layer|, |x|,
        // and |y|.
        void set_tilemap_data(uint8_t layer, uint8_t x, uint8_t y,
                              const void* tilemap_data, uint16_t size);

        // A wrapper around image offset lookup.
        uint16_t get_image_offset(int type, uint8_t index);

    private:
        // Updates a sprite in the sprite table given an updated entity object.
        void update_sprite(const GameEntities::GameEntity* object);
    };

}  // namespace Graphics

#endif  //SCREEN_H
