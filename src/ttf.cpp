/*
 ttf.cpp
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

#include "ttf.h"
#include <iostream>

#define NDEBUG

namespace Ui {

    TTFont::TTFont(const char* ttf_name, int size)
    {
        TTF_Init();
        font = TTF_OpenFont(ttf_name, size);
        if (font == NULL) {
            TTF_Quit();
            std::cout << ttf_name << '\n';
            throw "font initialization failure\n";
        }
    }
    TTFont::~TTFont()
    {
        typedef std::vector<SDL_Surface*>::iterator vIter;
        for (vIter it = surf_vec.begin(); it != surf_vec.end(); ++it) {
            SDL_FreeSurface(*it);
        }
        TTF_CloseFont(font);
        TTF_Quit();
    }
    SDL_Surface* TTFont::render_solid(const char* txt, Uint8 r, Uint8 g, Uint8 b)
    {
        fg_color.r = r; fg_color.g = g; fg_color.b = b;
        txt_surface = TTF_RenderText_Solid(font, txt, fg_color);
#ifndef NDEBUG
        if (txt_surface == NULL) throw "call to TTF_RenderText_Solid failed\n";
#endif
        surf_vec.push_back(txt_surface);
        return txt_surface;
    }
    SDL_Surface* TTFont::render_solid(const char* txt, SDL_Color color)
    {
        txt_surface = TTF_RenderText_Solid(font, txt, color);
#ifndef NDEBUG
        if (txt_surface == NULL) throw "call to TTF_RenderText_Solid failed\n";
#endif
        surf_vec.push_back(txt_surface);
        return txt_surface;
    }
    SDL_Surface* TTFont::render_shaded(const char* txt, Uint8 fg_r, Uint8 fg_g, Uint8 fg_b,
                                                        Uint8 bg_r, Uint8 bg_g, Uint8 bg_b)
    {
        fg_color.r = fg_r; fg_color.g = fg_g; fg_color.b = fg_b;
        bg_color.r = bg_r; bg_color.g = bg_g; bg_color.b = bg_b;
        txt_surface = TTF_RenderText_Shaded(font, txt, fg_color, bg_color);
#ifndef NDEBUG
        if (txt_surface == NULL) throw "call to TTF_RenderText_Shaded failed\n";
#endif
        surf_vec.push_back(txt_surface);
        return txt_surface;
    }
    SDL_Surface* TTFont::render_shaded(const char* txt, SDL_Color fg_color, SDL_Color bg_color)
    {
        txt_surface = TTF_RenderText_Shaded(font, txt, fg_color, bg_color);
#ifndef NDEBUG
        if (txt_surface == NULL) throw "call to TTF_RenderText_Shaded failed\n";
#endif
        surf_vec.push_back(txt_surface);
        return txt_surface;
    }
    SDL_Surface* TTFont::render_blended(const char* txt, Uint8 r, Uint8 g, Uint8 b)
    {
        fg_color.r = r; fg_color.g = g; fg_color.b = b;
        txt_surface = TTF_RenderText_Blended(font, txt, fg_color);
#ifndef NDEBUG
        if (txt_surface == NULL) throw "call to TTF_RenderText_Blended failed\n";
#endif
        surf_vec.push_back(txt_surface);
        return txt_surface;
    }
    SDL_Surface* TTFont::render_blended(const char* txt, SDL_Color color)
    {
        txt_surface = TTF_RenderText_Blended(font, txt, color);
#ifndef NDEBUG
        if (txt_surface == NULL) throw "call to TTF_RenderText_Blended failed\n";
#endif
        surf_vec.push_back(txt_surface);
        return txt_surface;
    }

}
