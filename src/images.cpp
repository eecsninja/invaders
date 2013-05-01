/*
 images.cpp
 Classic Invaders

 Copyright (c) 2013, Simon Que
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

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <string.h>

extern const char* datadir;

namespace Graphics {

    Images::Images() : num_images(0)
    {
        memset(images, 0, sizeof(images));
    }

    Images::~Images()
    {
        for (int i = 0; i < MAX_NUM_IMAGES; ++i) {
            if (images[i])
                SDL_FreeSurface(images[i]);
        }
    }

    bool Images::load_images(const char** filenames)
    {
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
        return true;
    }

    SDL_Surface *Images::get_image(const char* filename)
    {
        return get_image(get_image_index(filename));
    }

    SDL_Surface *Images::get_image(int index)
    {
        if (index < 0 || index >= MAX_NUM_IMAGES)
            return NULL;
        return images[index];
    }

    // Get index of image with file name.  Returns -1 if not found.
    int Images::get_image_index(const char* filename) {
        for (int i = 0; i < MAX_NUM_IMAGES && filenames[i]; ++i)
            if(strcmp(filename, filenames[i]) == 0)
                return i;
        return -1;
    }
}
