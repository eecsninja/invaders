/*
 images.cpp
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

#include "images.h"

#ifdef __i386__
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

#include <stdio.h>
#include <string.h>

#include "game_entity.h"

extern const char* datadir;

namespace Graphics {

    Images::Images() : num_images(0)
    {
        memset(images, 0, sizeof(images));
    }

    Images::~Images()
    {
#ifdef __i386__
        for (int i = 0; i < MAX_NUM_IMAGES; ++i) {
            if (images[i])
                SDL_FreeSurface(images[i]);
        }
#endif
    }

    bool Images::load_images(const char** filenames)
    {
#ifdef __i386__
        for (int i = 0; i < MAX_NUM_IMAGES && filenames[i]; ++i) {
            const char* filename = filenames[i];
            strcpy(this->filenames[i], filename);

            char path[1024];
            sprintf(path, "%s%s", datadir, filename);

            SDL_Surface* temp = IMG_Load(path);
            if (temp == NULL) {
                fprintf(stderr, "Unable to load image file: %s\n", filename);
                return false;
            }
            SDL_Surface* image = SDL_DisplayFormatAlpha(temp);
            images[i] = image;
            SDL_FreeSurface(temp);
        }
#endif  // defined(__i386__)
        return true;
    }

    SDL_Surface *Images::get_image(const char* filename)
    {
        int index = get_image_index(filename);
        if (index < 0 || index >= MAX_NUM_IMAGES) {
            fprintf(stderr, "Could not find image file %s.\n", filename);
            return NULL;
        }
        return images[index];
    }

    SDL_Surface *Images::get_image(int type, int index)
    {
        char base_name[100];
        switch(type) {
        case GAME_ENTITY_PLAYER:
            strcpy(base_name, "ship");
            break;
        case GAME_ENTITY_ALIEN:
        case GAME_ENTITY_ALIEN2:
        case GAME_ENTITY_ALIEN3:
            sprintf(base_name, "alien-%u-%u",
                    type - GAME_ENTITY_ALIEN + 1, index + 1);
            break;
        case GAME_ENTITY_BONUS_SHIP:
        case GAME_ENTITY_SMALL_BONUS_SHIP:
            sprintf(base_name, "bonus-%u-%u",
                    type - GAME_ENTITY_BONUS_SHIP + 1, index + 1);
            break;
        case GAME_ENTITY_SHOT:
            strcpy(base_name, "shot");
            break;
        case GAME_ENTITY_SHIELD_PIECE:
            strcpy(base_name, "shield_piece");
            break;
        case GAME_ENTITY_EXPLOSION:
            strcpy(base_name, "explosion");
            break;
        case GAME_ENTITY_UNKNOWN:
        case GAME_ENTITY_SHIELD_GROUP:
        default:
            fprintf(stderr, "Attempting to get image for invalid type: %u\n",
                    type);
            return NULL;
        }
        strcat(base_name, ".png");
        return get_image(base_name);
    }

    // Get index of image with file name.  Returns -1 if not found.
    int Images::get_image_index(const char* filename) {
#ifdef __i386__
        for (int i = 0; i < MAX_NUM_IMAGES && filenames[i]; ++i)
            if(strcmp(filename, filenames[i]) == 0)
                return i;
#endif
        return -1;
    }
}
