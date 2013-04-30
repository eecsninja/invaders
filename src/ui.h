/*
 ui.h
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

#ifndef UI_H
#define UI_H

#include "ttf.h"
#include "sound.h"
#include <SDL/SDL.h>
#include <vector>
#include <string>

namespace Game {
    class Game;
}

namespace Ui {

    typedef struct {
        std::string name;
        Uint32 score;
        int wave;
        bool just_added;
    } score_rec;
    bool compare(const score_rec& x, const score_rec& y);

    class Ui {
        Sound::Sound* sound;
        Game::Game* game;
        int fullscreen_flag;
        std::vector<score_rec> high_scores;
        std::string score_file, serif_bold, mono_regular;
        TTFont TitleFont, MenuFont;
        SDL_Surface *title, *play, *r_play, *quit, *r_quit, *zap, *r_zap, *help, *r_help, *about, *r_about;
        SDL_Surface *window, *fullscreen, *toggle, *r_window, *r_fullscreen, *r_toggle;
        SDL_Color fg_color, bg_color;
        SDL_Rect src, dst;
        const static Uint8 score_list_len = 10;
        int top_menu_y, left_margin_x, menu_w, menu_h;
        void blit_menu_button(SDL_Surface* menu_button, int pos);
        void blit_high_scores();
        void blit_name_input(const std::string& name, TTFont& ttf, int y);
        void about_screen();
        void write_scores();
        void return_msg();
        void help_screen();
        void toggle_fullscreen();
        bool score_file_available;
    public:
        Ui(Sound::Sound* sound, Game::Game* game, int fullscreen_flag);
        ~Ui() { write_scores(); }
        bool start_screen();
        void check_high_scores(Uint32 score, int wave);
        void screen_msg(const char* msg);
        void wave_msg(int wave);
    };

}
#endif  //UI_H
