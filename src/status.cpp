/*
 status.cpp
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

#include "status.h"
#include "screen.h"
#include <sstream>

extern std::string datadir;

namespace Ui {

    Status::Status()
    {
        const char* font_file = "LiberationSans-Bold.ttf";
        std::string fontpath = datadir + font_file;
        TTF_Init();
        font = TTF_OpenFont(fontpath.c_str(), 25);
        if (font == NULL) {
            TTF_Quit();
            std::ostringstream s;
            s << "font initialization failure in class Status\n" <<
                "Unable to load font file: " << font_file << '\n';
            throw s.str();
        }
        fg_color.r = 0; fg_color.g = 100; fg_color.b = 150;
        bg_color.r = 0; bg_color.g = 0; bg_color.b = 0;
        src.x = src.y = 0;
    }
    Status::~Status()
    {
        TTF_CloseFont(font);
        TTF_Quit();
    }
    void Status::blit_score(Uint32 score)
    {
        std::ostringstream s;
        s << "Score: " << score;
        str = s.str();
        surface = TTF_RenderText_Shaded(font, str.c_str(), fg_color, bg_color);
        src.w = surface->w; src.h = surface->h;
        dst.x = (screen_w - src.w) / 2; dst.y = top_margin_y;
        dst.w = src.w; dst.h = src.h;
        SDL_SetClipRect(screen, NULL);
        SDL_BlitSurface(surface, &src, screen, &dst);
        SDL_UpdateRect(screen, dst.x, dst.y, dst.w, dst.h);
        SDL_SetClipRect(screen, &clip);
        SDL_FreeSurface(surface);
    }
    void Status::blit_wave(int wave)
    {
        std::ostringstream s;
        s << "Wave: " << wave;
        str = s.str();
        surface = TTF_RenderText_Shaded(font, str.c_str(), fg_color, bg_color);
        src.w = surface->w; src.h = surface->h;
        dst.x = screen_w - src.w - text_padding; dst.y = top_margin_y;
        dst.w = src.w; dst.h = src.h;
        SDL_SetClipRect(screen, NULL);
        SDL_BlitSurface(surface, &src, screen, &dst);
        SDL_UpdateRect(screen, dst.x, dst.y, dst.w, dst.h);
        SDL_SetClipRect(screen, &clip);
        SDL_FreeSurface(surface);
    }

    void Status::blit_player_ships(int n, SDL_Surface* ship)
    {
        src.w = ship->w + ship_padding; src.h = ship->h;
        dst.w = src.w; dst.h = src.h;
        SDL_SetClipRect(screen, NULL);
        int count = n;
        while (--count) {
            dst.x = count*src.w - ship->w; dst.y = bottom_margin_y;
            SDL_BlitSurface(ship, &src, screen, &dst);
        }
        SDL_UpdateRect(screen, 0, bottom_margin_y, n*dst.w + ship_padding*n, dst.h);
        SDL_SetClipRect(screen, &clip);
    }
    void Status::erase_player_ship(int n, SDL_Surface* ship)
    {
        src.w = ship->w + ship_padding; src.h = ship->h;
        dst.x = n*src.w - ship->w; dst.y = bottom_margin_y;
        dst.w = src.w; dst.h = src.h;
        SDL_SetClipRect(screen, NULL);
        SDL_BlitSurface(wave_background, &src, screen, &dst);
        SDL_UpdateRect(screen, 0, dst.y, dst.x + dst.w, dst.h);
        SDL_SetClipRect(screen, &clip);
    }
    void Status::blit_lives(int n)
    {
        std::ostringstream s;
        // pad end of string in case number of lives reaches 10
        s << "Lives: " << n << "  ";
        str = s.str();
        surface = TTF_RenderText_Shaded(font, str.c_str(), fg_color, bg_color);
        src.w = surface->w; src.h = surface->h;
        dst.x = text_padding; dst.y = top_margin_y;
        dst.w = src.w; dst.h = src.h;
        SDL_SetClipRect(screen, NULL);
        SDL_BlitSurface(surface, &src, screen, &dst);
        SDL_UpdateRect(screen, dst.x, dst.y, dst.w, dst.h);
        SDL_SetClipRect(screen, &clip);
        SDL_FreeSurface(surface);
    }

}
