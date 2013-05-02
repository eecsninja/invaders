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

#include <SDL/SDL.h>
#include <stdlib.h>

namespace Graphics {

    void set_video_mode(bool fullscreen) {
        int flags = SDL_SWSURFACE;
        if (fullscreen)
            flags |= SDL_FULLSCREEN;
        screen = SDL_SetVideoMode(screen_w, screen_h, 16, flags);
        if (screen == NULL) {
            fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
            exit(1);
        }
    }

    void schedule_blit(SDL_Surface* image, int x, int y) {
        if (screen_updates >= max_updates) {
            fprintf(stderr, "Exceeded max number of blits (%d).\n", max_updates);
            return;
        }

        blit* update = &blits[screen_updates++];
        update->img = image;

        update->src.x = 0;
        update->src.y = 0;
        update->src.w = image->w;
        update->src.h = image->h;

        update->dst.x = x;
        update->dst.y = y;
        update->dst.w = image->w;
        update->dst.h = image->h;
    }

    void flush_blits() {
        for (int i = 0; i < screen_updates; ++i)
            SDL_BlitSurface(blits[i].img, &blits[i].src, screen, &blits[i].dst);
        screen_updates = 0;
        SDL_UpdateRect(screen, clip.x, clip.y, clip.w, clip.h);
    }

    void update_screen() {
        SDL_BlitSurface(wave_background, NULL, screen, NULL);
        flush_blits();
    }
}
