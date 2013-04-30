/*
 ttf.h
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

#ifndef TTF_H
#define TTF_H

#include "SDL_ttf.h"
#include "SDL.h"
#include <vector>

namespace Ui {

    class TTFont {
        TTF_Font* font;
        SDL_Surface* txt_surface;
        SDL_Color fg_color, bg_color;
        std::vector<SDL_Surface*> surf_vec;
    public:
        TTFont(const char* ttf_name, int size);
        ~TTFont();
        SDL_Surface* render_solid(const char* txt, Uint8 r, Uint8 g, Uint8 b);
        SDL_Surface* render_solid(const char* txt, SDL_Color color);
        SDL_Surface* render_shaded(const char* txt, Uint8 fg_r, Uint8 fg_g, Uint8 fg_b,
                                                    Uint8 bg_r, Uint8 bg_g, Uint8 bg_b);
        SDL_Surface* render_shaded(const char* txt, SDL_Color fg_color, SDL_Color bg_color);
        SDL_Surface* render_blended(const char* txt, Uint8 r, Uint8 g, Uint8 b);
        SDL_Surface* render_blended(const char* txt, SDL_Color color);
    };

}
#endif  //TTF_H
