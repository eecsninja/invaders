/*
 ui.cpp
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

#include "ui.h"
#include "screen.h"
#include "game.h"
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <cstdlib>

extern std::string datadir;

namespace Ui {

    bool compare(const score_rec& x, const score_rec& y) { return x.score > y.score; }

    Ui::Ui(Sound::Sound* sound, Game::Game* game, int fullscreen_flag) :
        sound(sound),
        game(game),
        fullscreen_flag(fullscreen_flag),
        serif_bold(datadir + "LiberationSerif-Bold.ttf"),
        mono_regular(datadir + "LiberationMono-Regular.ttf"),
        TitleFont(serif_bold.c_str(), 65),
        MenuFont(mono_regular.c_str(), 25)
    {
#ifdef AUTOTOOLS_INSTALL
        std::ostringstream ss;
        if (ss << getenv("HOME")) {
            score_file = ss.str() + "/.classic-invaders-scores";
            score_file_available = true;
        } else {
            std::cout << "Could not locate $HOME dir to save score file to." << '\n'
                      << "Scores will not be saved." << '\n';
            score_file_available = false;
        }
#else
        score_file = datadir + "classic-invaders-scores";
#endif
        // read high scores into vector
        std::ifstream in(score_file.c_str());
        score_rec s;
        if (in) {
            while (in >> s.name) {
                in >> s.score;
                in >> s.wave;
                s.just_added = false;
                high_scores.push_back(s);
            }
            in.close();
        }
        // title and menu surfaces and colors
        fg_color.r = 0; fg_color.g = 100; fg_color.b = 150;
        bg_color.r = 0; bg_color.g = 0; bg_color.b = 0;
        title = TitleFont.render_blended("CLASSIC INVADERS", fg_color);
        play = MenuFont.render_shaded(" Play       ", fg_color, bg_color);
        quit = MenuFont.render_shaded(" Quit       ", fg_color, bg_color);
        zap = MenuFont.render_shaded(" Zap Scores ", fg_color, bg_color);
        help = MenuFont.render_shaded(" Help       ", fg_color, bg_color);
        about = MenuFont.render_shaded(" About      ", fg_color, bg_color);
        window = MenuFont.render_shaded(" Window    ", fg_color, bg_color);
        fullscreen = MenuFont.render_shaded(" Fullscreen ", fg_color, bg_color);
        // menu surfaces in reverse video
        r_play = MenuFont.render_shaded(" Play       ", bg_color, fg_color);
        r_quit = MenuFont.render_shaded(" Quit       ", bg_color, fg_color);
        r_zap = MenuFont.render_shaded(" Zap Scores ", bg_color, fg_color);
        r_help = MenuFont.render_shaded(" Help       ", bg_color, fg_color);
        r_about = MenuFont.render_shaded(" About      ", bg_color, fg_color);
        r_window = MenuFont.render_shaded(" Window    ", bg_color, fg_color);
        r_fullscreen = MenuFont.render_shaded(" Fullscreen ", bg_color, fg_color);
    }
    bool Ui::start_screen()
    {
        enum state { PLAY, QUIT, TOGGLE, ZAP, HELP, ABOUT } current;
        current = PLAY;
        sound->play_start_screen();
        main_menu:
        if (fullscreen_flag) {
            toggle = window;
            r_toggle = r_window;
        } else {
            toggle = fullscreen;
            r_toggle = r_fullscreen;
        }
        // blit title
        SDL_SetClipRect(screen, NULL);
        SDL_BlitSurface(background, NULL, screen, NULL);
        src.x = src.y = 0; src.w = ui_header->w; src.h = ui_header->h;
        // starting x and y coordinates
        left_margin_x = dst.x = (screen_w - src.w) / 2; dst.y = 50;
        top_menu_y = dst.y + (Sint16)(ui_header->h*1.2);
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(ui_header, &src, screen, &dst);
        // menu dimensions
        menu_h = play->h + quit->h + toggle->h + zap->h + help->h + about->h;
        // menu width should be left margin plus longest menu surface
        menu_w = left_margin_x + zap->w;
        blit_high_scores();
        // adjust x and y for menu
        top_menu_y = top_menu_y - 20;
        left_margin_x = left_margin_x + 20;
        SDL_UpdateRect(screen,0,0,0,0);
        switch (current) {
            case PLAY:
                blit_menu_button(r_play, 0);
                blit_menu_button(quit, 1);
                blit_menu_button(toggle, 2);
                blit_menu_button(zap, 3);
                blit_menu_button(help, 4);
                blit_menu_button(about, 5);
                SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                break;
            case QUIT:
                blit_menu_button(play, 0);
                blit_menu_button(r_quit, 1);
                blit_menu_button(toggle, 2);
                blit_menu_button(zap, 3);
                blit_menu_button(help, 4);
                blit_menu_button(about, 5);
                SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                break;
            case TOGGLE:
                blit_menu_button(play, 0);
                blit_menu_button(quit, 1);
                blit_menu_button(r_toggle, 2);
                blit_menu_button(zap, 3);
                blit_menu_button(help, 4);
                blit_menu_button(about, 5);
                SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                break;
            case ZAP:
                blit_menu_button(play, 0);
                blit_menu_button(quit, 1);
                blit_menu_button(toggle, 2);
                blit_menu_button(r_zap, 3);
                blit_menu_button(help, 4);
                blit_menu_button(about, 5);
                SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                break;
            case HELP:
                blit_menu_button(play, 0);
                blit_menu_button(quit, 1);
                blit_menu_button(toggle, 2);
                blit_menu_button(zap, 3);
                blit_menu_button(r_help, 4);
                blit_menu_button(about, 5);
                SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                break;
            case ABOUT:
                blit_menu_button(play, 0);
                blit_menu_button(quit, 1);
                blit_menu_button(toggle, 2);
                blit_menu_button(zap, 3);
                blit_menu_button(help, 4);
                blit_menu_button(r_about, 5);
                SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                break;
            default:
                break;
        }
        SDL_Event event;
        Uint8* keys;
        while (1) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    return false;
                }
                keys = SDL_GetKeyState(NULL);
                if (current == PLAY && (keys[SDLK_DOWN] || keys[SDLK_TAB])) {
                    sound->play_menu_select();
                    blit_menu_button(play, 0);
                    blit_menu_button(r_quit, 1);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = QUIT;
                    break;
                }
                if (current == QUIT && keys[SDLK_UP]) {
                    sound->play_menu_select();
                    blit_menu_button(r_play, 0);
                    blit_menu_button(quit, 1);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = PLAY;
                    break;
                }
                if (current == QUIT && (keys[SDLK_DOWN] || keys[SDLK_TAB])) {
                    sound->play_menu_select();
                    blit_menu_button(quit, 1);
                    blit_menu_button(r_toggle, 2);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = TOGGLE;
                    break;
                }
                if (current == TOGGLE && keys[SDLK_UP]) {
                    sound->play_menu_select();
                    blit_menu_button(r_quit, 1);
                    blit_menu_button(toggle, 2);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = QUIT;
                    break;
                }
                if (current == TOGGLE && (keys[SDLK_DOWN] || keys[SDLK_TAB])) {
                    sound->play_menu_select();
                    blit_menu_button(toggle, 2);
                    blit_menu_button(r_zap, 3);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = ZAP;
                    break;
                }
                if (current == ZAP && keys[SDLK_UP]) {
                    sound->play_menu_select();
                    blit_menu_button(r_toggle, 2);
                    blit_menu_button(zap, 3);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = TOGGLE;
                    break;
                }
                if (current == ZAP && (keys[SDLK_DOWN] || keys[SDLK_TAB])) {
                    sound->play_menu_select();
                    blit_menu_button(zap, 3);
                    blit_menu_button(r_help, 4);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = HELP;
                    break;
                }
                if (current == HELP && keys[SDLK_UP]) {
                    sound->play_menu_select();
                    blit_menu_button(r_zap, 3);
                    blit_menu_button(help, 4);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = ZAP;
                    break;
                }
                if (current == HELP && (keys[SDLK_DOWN] || keys[SDLK_TAB])) {
                    sound->play_menu_select();
                    blit_menu_button(help, 4);
                    blit_menu_button(r_about, 5);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = ABOUT;
                    break;
                }
                if (current == ABOUT && keys[SDLK_UP]) {
                    sound->play_menu_select();
                    blit_menu_button(r_help, 4);
                    blit_menu_button(about, 5);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = HELP;
                    break;
                }
                if (current == ABOUT && keys[SDLK_TAB]) {
                    sound->play_menu_select();
                    blit_menu_button(r_play, 0);
                    blit_menu_button(about, 5);
                    SDL_UpdateRect(screen, left_margin_x, top_menu_y, menu_w, menu_h);
                    current = PLAY;
                    break;
                }
                if (keys[SDLK_ESCAPE]) {
                    return false;
                }
                if (current == QUIT && (keys[SDLK_RETURN] || keys[SDLK_SPACE])) {
                    sound->play_exit();
                    return false;
                }
                if (current == PLAY && (keys[SDLK_RETURN] || keys[SDLK_SPACE])) {
                    sound->halt_all_sounds();
                    return true;
                }
                if (current == ZAP && (keys[SDLK_RETURN] || keys[SDLK_SPACE])) {
                    sound->play_zap_scores();
                    high_scores.clear();
                    write_scores();
                    goto main_menu;
                }
                if (current == TOGGLE && (keys[SDLK_RETURN] || keys[SDLK_SPACE])) {
                    toggle_fullscreen();
                    SDL_Delay(500);
                    current = PLAY;
                    goto main_menu;
                }
                if (current == HELP && (keys[SDLK_RETURN] || keys[SDLK_SPACE])) {
                    help_screen();
                    sound->play_help_screen();
                    while (SDL_WaitEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            return false;
                        }
                        keys = SDL_GetKeyState(NULL);
                        if (keys[SDLK_RETURN] || keys[SDLK_SPACE] || keys[SDLK_ESCAPE]) {
                            sound->halt_all_sounds();
                            goto main_menu;
                        }
                    }
                }
                if (current == ABOUT && (keys[SDLK_RETURN] || keys[SDLK_SPACE])) {
                    about_screen();
                    sound->play_about_screen();
                    while (SDL_WaitEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            return false;
                        }
                        keys = SDL_GetKeyState(NULL);
                        if (keys[SDLK_RETURN] || keys[SDLK_SPACE] || keys[SDLK_ESCAPE]) {
                            sound->halt_all_sounds();
                            goto main_menu;
                        }
                    }
                }
            }
        }
    }
    void Ui::check_high_scores(Uint32 score, int wave)
    {
        if (score == 0) return;
        Uint32 s;
        if (high_scores.empty()) {
            s = 0;
        } else {
            s = high_scores[high_scores.size()-1].score;
        }
        if (high_scores.size() == score_list_len){
            if (score <= s) return;
        }
        sound->play_new_high_score();
        // blit high score message
        SDL_SetClipRect(screen, NULL);
        SDL_BlitSurface(background, NULL, screen, NULL);
        const char* ch = "New High Score!";
        TTFont ttf(mono_regular.c_str(), 20);
        SDL_Surface* surface = ttf.render_blended(ch, fg_color);
        src.x = src.y = 0; src.w = surface->w; src.h = surface->h;
        dst.x = (screen_w - src.w) / 2;
        int y = dst.y = (screen_h - (src.h*6)) / 2;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surface, &src, screen, &dst);
        // blit prompt
        ch = "Enter name: __________";
        surface = ttf.render_blended(ch, fg_color);
        src.w = surface->w; src.h = surface->h;
        dst.x = (screen_w - src.w) / 2; dst.y = y + 40;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surface, &src, screen, &dst);
        SDL_UpdateRect(screen,0,0,0,0);
        // get player name
        std::string name(ch);
        Uint8 pos = 12;
        score_rec s_rec;
        SDL_Event event;
        while (SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            sound->halt_all_sounds();
                            return;
                            break;
                        case SDLK_a:
                            if (pos < name.size()) {
                                name[pos++] = 'A';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_b:
                            if (pos < name.size()) {
                                name[pos++] = 'B';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_c:
                            if (pos < name.size()) {
                                name[pos++] = 'C';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_d:
                            if (pos < name.size()) {
                                name[pos++] = 'D';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_e:
                            if (pos < name.size()) {
                                name[pos++] = 'E';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_f:
                            if (pos < name.size()) {
                                name[pos++] = 'F';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_g:
                            if (pos < name.size()) {
                                name[pos++] = 'G';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_h:
                            if (pos < name.size()) {
                                name[pos++] = 'H';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_i:
                            if (pos < name.size()) {
                                name[pos++] = 'I';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_j:
                            if (pos < name.size()) {
                                name[pos++] = 'J';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_k:
                            if (pos < name.size()) {
                                name[pos++] = 'K';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_l:
                            if (pos < name.size()) {
                                name[pos++] = 'L';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_m:
                            if (pos < name.size()) {
                                name[pos++] = 'M';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_n:
                            if (pos < name.size()) {
                                name[pos++] = 'N';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_o:
                            if (pos < name.size()) {
                                name[pos++] = 'O';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_p:
                            if (pos < name.size()) {
                                name[pos++] = 'P';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_q:
                            if (pos < name.size()) {
                                name[pos++] = 'Q';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_r:
                            if (pos < name.size()) {
                                name[pos++] = 'R';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_s:
                            if (pos < name.size()) {
                                name[pos++] = 'S';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_t:
                            if (pos < name.size()) {
                                name[pos++] = 'T';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_u:
                            if (pos < name.size()) {
                                name[pos++] = 'U';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_v:
                            if (pos < name.size()) {
                                name[pos++] = 'V';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_w:
                            if (pos < name.size()) {
                                name[pos++] = 'W';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_x:
                            if (pos < name.size()) {
                                name[pos++] = 'X';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_y:
                            if (pos < name.size()) {
                                name[pos++] = 'Y';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_z:
                            if (pos < name.size()) {
                                name[pos++] = 'Z';
                                blit_name_input(name, ttf, y);
                            } else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_BACKSPACE:
                            if (pos > 12) {
                                name[--pos] = '_';
                                blit_name_input(name, ttf, y);
                            }
                            else {
                                sound->play_wrong_key();
                            }
                            break;
                        case SDLK_RETURN:
                            if (name == ch) {
                                s_rec.name = "----------";
                            } else {
                                name.erase(pos);
                                s_rec.name = name.substr(12, pos);
                            }
                            s_rec.score = score;
                            s_rec.wave = wave;
                            s_rec.just_added = true;
                            high_scores.push_back(s_rec);
                            std::stable_sort(high_scores.begin(), high_scores.end(), compare);
                            if (high_scores.size() > score_list_len) {
                                high_scores.pop_back();
                            }
                            sound->halt_all_sounds();
                            return;
                            break;
                        default:
                            sound->play_wrong_key();
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    inline void Ui::blit_name_input(const std::string& name, TTFont& ttf, int y)
    {
        const char* name_char;
        name_char = name.c_str();
        SDL_Surface* surface = ttf.render_blended(name_char, fg_color);
        src.x = src.y = 0; src.w = surface->w; src.h = surface->h;
        dst.x = (screen_w - src.w) / 2; dst.y = y + 40;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(background, &src, screen, &dst);
        SDL_BlitSurface(surface, &src, screen, &dst);
        SDL_UpdateRect(screen, dst.x, dst.y, dst.w, dst.h);
    }
    inline void Ui::blit_high_scores()
    {
        std::ostringstream s, l;
        s << std::setw(13) << std::left << "Name"
            << std::setw(12) << std::right << "Score"
            << std::setw(12) << std::right << "Wave";
        l << std::setw(13) << std::left << "===="
            << std::setw(12) << std::right << "====="
            << std::setw(12) << std::right << "====";
        std::string str = s.str();
        const char* heading = str.c_str();
        TTFont ttf(mono_regular.c_str(), 20);
        // blit heading
        SDL_Surface* heading_surf = ttf.render_blended(heading, fg_color);
        src.x = src.y = 0; src.w = heading_surf->w; src.h = heading_surf->h;
        dst.x = left_margin_x + menu_w; dst.y = top_menu_y - 20;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(heading_surf, &src, screen, &dst);
        // blit heading underline
        str = l.str();
        heading = str.c_str();
        heading_surf = ttf.render_blended(heading, fg_color);
        src.w = heading_surf->w; src.h = heading_surf->h;
        dst.y = top_menu_y;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(heading_surf, &src, screen, &dst);
        // default entry
        if (high_scores.empty()) {
            std::ostringstream s;
            s << std::setw(13) << std::left << "PLAYERNAME"
                << std::setw(12) << std::right <<  0
                << std::setw(12) << std::right << 0;
            str = s.str();
            const char* score_line = str.c_str();
            SDL_Surface* score_surf;
            score_surf = ttf.render_blended(score_line, fg_color);
            src.w = score_surf->w; src.h = score_surf->h;
            dst.x = left_margin_x + menu_w; dst.y = top_menu_y + heading_surf->h + (src.h * 0);
            dst.w = src.w; dst.h = src.h;
            SDL_BlitSurface(score_surf, &src, screen, &dst);
            return;
        }
        int next_y = 0;
        for (std::vector<score_rec>::iterator it = high_scores.begin(); it != high_scores.end(); ++it) {
            std::ostringstream s;
            s << std::setw(13) << std::left << (*it).name
                << std::setw(12) << std::right <<  (*it).score
                << std::setw(12) << std::right << (*it).wave;
            str = s.str();
            const char* score_line = str.c_str();
            SDL_Surface* score_surf;
            if ((*it).just_added) {
                score_surf = ttf.render_blended(score_line, 200, 0, 0);
                (*it).just_added = false;
            } else {
                score_surf = ttf.render_blended(score_line, fg_color);
            }
            src.w = score_surf->w; src.h = score_surf->h;
            dst.x = left_margin_x + menu_w; dst.y = top_menu_y + heading_surf->h + (src.h * next_y++);
            dst.w = src.w; dst.h = src.h;
            SDL_BlitSurface(score_surf, &src, screen, &dst);
        }
    }
    inline void Ui::blit_menu_button(SDL_Surface* menu_button, int pos)
    {
        src.x = src.y = 0;
        src.w = play->w;
        src.h = play->h;
        dst.x = left_margin_x;
        dst.y = top_menu_y + play->h * pos;
        dst.w = src.w;
        dst.h = src.h;
        SDL_BlitSurface(menu_button, &src, screen, &dst);
    }
    inline void Ui::return_msg()
    {
        TTFont ttf(serif_bold.c_str(), 20);
        const char* text = "Press Space to Return.";
        SDL_Surface* surf = ttf.render_blended(text, fg_color);
        src.x = src.y = 0; src.w = surf->w; src.h = surf->h;
        dst.x = (screen_w - src.w) / 2; dst.y = screen_h - (src.h*2);
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surf, &src, screen, &dst);
    }
    void Ui::help_screen()
    {
        // blit title
        SDL_BlitSurface(background, NULL, screen, NULL);
        src.x = src.y = 0; src.w = ui_header->w; src.h = ui_header->h;
        dst.x = (screen_w - src.w) / 2; dst.y = 50;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(ui_header, &src, screen, &dst);
        // blit help messages
        dst.x = dst.x = 100;
        TTFont help_msg(serif_bold.c_str(), 25);
        const char*  text = "Spacebar to fire.";
        SDL_Surface* surf = help_msg.render_blended(text, fg_color);
        src.w = surf->w; src.h = surf->h;
        dst.y = (Sint16)(screen_h / 3.5);
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surf, &src, screen, &dst);
        text = "<  > Arrow keys to move.";
        surf = help_msg.render_blended(text, fg_color);
        src.w = surf->w; src.h = surf->h;
        dst.y = (Sint16)(screen_h / 3.5) + src.h;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surf, &src, screen, &dst);
        text = "Bonus ship every 50000.";
        surf = help_msg.render_blended(text, fg_color);
        src.w = surf->w; src.h = surf->h;
        dst.y = (Sint16)(screen_h / 3.5) + src.h*2;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surf, &src, screen, &dst);
        text = "Press 'p' to pause a wave, press again to continue.";
        surf = help_msg.render_blended(text, fg_color);
        src.w = surf->w; src.h = surf->h;
        dst.y = (Sint16)(screen_h / 3.5) + src.h*3;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surf, &src, screen, &dst);
        src.w = ui_points->w; src.h = ui_points->h;
        dst.y = 300;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(ui_points, &src, screen, &dst);
        return_msg();
        SDL_UpdateRect(screen,0,0,0,0);
    }
    void Ui::about_screen()
    {
        // blit title
        SDL_BlitSurface(background, NULL, screen, NULL);
        src.x = src.y = 0; src.w = ui_header->w; src.h = ui_header->h;
        dst.x = (screen_w - src.w) / 2; dst.y = 50;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(ui_header, &src, screen, &dst);
        // blit credits
        TTFont credits(serif_bold.c_str(), 35);
        const char* text = "Programming and graphics by";
        SDL_Surface* surf = credits.render_blended(text, fg_color);
        src.w = surf->w; src.h = surf->h;
        dst.x = (screen_w - src.w) / 2; dst.y = screen_h / 3;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surf, &src, screen, &dst);
        text = "Todd Steinackle";
        surf = credits.render_blended(text, fg_color);
        src.w = surf->w; src.h = surf->h;
        dst.x = (screen_w - src.w) / 2; dst.y = screen_h / 3 + (Sint16)(src.h*1.5);
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surf, &src, screen, &dst);
        TTFont url(serif_bold.c_str(), 30);
        text = "www.noquarterarcade.com";
        surf = url.render_blended(text, fg_color);
        src.w = surf->w; src.h = surf->h;
        dst.x = (screen_w - src.w) / 2; dst.y = screen_h / 3 + src.h*5;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(surf, &src, screen, &dst);
        return_msg();
        SDL_UpdateRect(screen,0,0,0,0);
    }
    void Ui::write_scores()
    {
        // write high scores file
        if (score_file_available) {
            std::ofstream out(score_file.c_str());
            if (!out) throw "could not write high score file\n";
            for (std::vector<score_rec>::iterator it = high_scores.begin(); it != high_scores.end(); ++it) {
                out << (*it).name << " " << (*it).score << " " << (*it).wave << '\n';
            }
            out.close();
        }
    }
    void Ui::wave_msg(int wave)
    {
        std::ostringstream s;
        s << "Prepare for wave " << wave << "!";
        std::string str = s.str();
        const char* wave_char = str.c_str();
        SDL_SetClipRect(screen, NULL);
        SDL_BlitSurface(background, NULL, screen, NULL);
        TTFont ttf(serif_bold.c_str(), 50);
        SDL_Surface* wave_surf = ttf.render_blended(wave_char, fg_color);
        src.x = src.y = 0; src.w = wave_surf->w; src.h = wave_surf->h;
        dst.x = (screen_w - src.w) / 2; dst.y = (screen_h - (src.h*3)) / 2;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(wave_surf, &src, screen, &dst);
        SDL_UpdateRect(screen,0,0,0,0);
    }
    void Ui::screen_msg(const char* msg)
    {
        SDL_SetClipRect(screen, NULL);
        SDL_BlitSurface(background, NULL, screen, NULL);
        TTFont ttf(serif_bold.c_str(), 50);
        SDL_Surface *wave = ttf.render_blended(msg, fg_color);
        src.x = src.y = 0; src.w = wave->w; src.h = wave->h;
        dst.x = (screen_w - src.w) / 2; dst.y = (screen_h - (src.h*3)) / 2;
        dst.w = src.w; dst.h = src.h;
        SDL_BlitSurface(wave, &src, screen, &dst);
        SDL_UpdateRect(screen,0,0,0,0);
    }
    void Ui::toggle_fullscreen()
    {
        if (fullscreen_flag) {
            fullscreen_flag = 0;
        } else {
            fullscreen_flag = 1;
        }
        game->set_video_mode(fullscreen_flag);
    }

}
