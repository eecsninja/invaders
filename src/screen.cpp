/*
 screen.cpp
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

#include "screen.h"

#include <stdlib.h>

#include "images.h"

namespace Graphics {

    bool Video::init() {
        atexit(SDL_Quit);
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
            fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
            return false;
        }
    }

    void Video::set_image_lib(Images* images) {
        image_lib = images;

        wave_background = images->get_image("wave_background.png");
        background = images->get_image("background.png");
        ui_header = images->get_image("ui_header.png");
        ui_points = images->get_image("ui_points.png");
    }

    void Video::set_video_mode(bool fullscreen) {
        int flags = SDL_SWSURFACE;
        if (fullscreen)
            flags |= SDL_FULLSCREEN;
        screen = SDL_SetVideoMode(screen_w, screen_h, 16, flags);
        if (screen == NULL) {
            fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
            exit(1);
        }

        clip.x = 0; clip.y = 50; clip.w = screen_w; clip.h = 515;
        SDL_SetClipRect(screen, &clip);
    }

    void Video::schedule_blit(int image_index, int x, int y) {
        if (num_blits >= max_updates) {
            fprintf(stderr, "Exceeded max number of blits (%d).\n", max_updates);
            return;
        }

        blit* update = &blits[num_blits++];
        update->image_index = image_index;

        update->src.x = 0;
        update->src.y = 0;
        update->src.w = screen_w;
        update->src.h = screen_h;

        update->dst.x = x;
        update->dst.y = y;
        update->dst.w = screen_w;
        update->dst.h = screen_h;
    }

    void Video::flush_blits() {
        for (int i = 0; i < num_blits && image_lib; ++i) {
            SDL_Surface* image = image_lib->get_image(blits[i].image_index);
            if (image)
                SDL_BlitSurface(image, &blits[i].src, screen, &blits[i].dst);
        }
        num_blits = 0;
        SDL_UpdateRect(screen, clip.x, clip.y, clip.w, clip.h);
    }

    void Video::update_screen() {
        SDL_BlitSurface(wave_background, NULL, screen, NULL);
        flush_blits();
    }
}
