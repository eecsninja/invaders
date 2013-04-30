/*
 sound.h
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

#ifndef SOUND_H
#define SOUND_H

#include "SDL_mixer.h"
#include <vector>

namespace Sound {

    class Sound {
        enum { SHOT, EXPLOSION, ALIEN_DEATH, FREE_GUY, GAME_OVER, START_WAVE, BIG_BONUS, SMALL_BONUS, SHOT_COLLISION,
                ACTIVE_BONUS, START_SCREEN, PLAYER_DEAD, PLAYER_REBIRTH, ALIENS_LANDED, END_WAVE, MENU_SELECT, ZAP_SCORES,
                NEW_HIGH_SCORE, ABOUT_SCREEN, WRONG_KEY, HELP_SCREEN, EXIT, BG, BG_4, BG_3, BG_2, BG_1 };
        std::vector<Mix_Chunk*> sounds;
    public:
        Sound();
        ~Sound();
        void free_sounds();
        void wait_for_all_to_finish() { while (Mix_Playing(-1)) ; }
        void halt_all_sounds() { Mix_HaltChannel(-1); }
        void play_shot() { Mix_PlayChannel(SHOT, sounds[SHOT], 0); }
        void play_explode() { Mix_PlayChannel(EXPLOSION, sounds[EXPLOSION], 0); }
        void play_alien_dead() { Mix_PlayChannel(ALIEN_DEATH, sounds[ALIEN_DEATH], 0); }
        void play_free_guy() { Mix_PlayChannel(FREE_GUY, sounds[FREE_GUY], 0); }
        void play_sbonus() { Mix_PlayChannel(BIG_BONUS, sounds[BIG_BONUS], 0); }
        void play_rbonus() { Mix_PlayChannel(SMALL_BONUS, sounds[SMALL_BONUS], 0); }
        void play_shot_collision() { Mix_PlayChannel(SHOT_COLLISION, sounds[SHOT_COLLISION], 0); }
        void play_start_screen() { Mix_PlayChannel(START_SCREEN, sounds[START_SCREEN], 0); }
        void play_new_high_score() { Mix_PlayChannel(NEW_HIGH_SCORE, sounds[NEW_HIGH_SCORE], 0); }
        void play_menu_select() {
            if (Mix_Playing(START_SCREEN)) {
                Mix_HaltChannel(START_SCREEN);
            }
            Mix_PlayChannel(MENU_SELECT, sounds[MENU_SELECT], 0);
        }
        void play_zap_scores() {
            if (Mix_Playing(START_SCREEN)) {
                Mix_HaltChannel(START_SCREEN);
            }
            Mix_PlayChannel(ZAP_SCORES, sounds[ZAP_SCORES], 0);
        }
        void play_about_screen() {
            if (Mix_Playing(START_SCREEN)) {
                Mix_HaltChannel(START_SCREEN);
            }
            Mix_PlayChannel(ABOUT_SCREEN, sounds[ABOUT_SCREEN], 0);
        }
        void play_wrong_key() {
            if (Mix_Playing(START_SCREEN)) {
                Mix_HaltChannel(START_SCREEN);
            }
            Mix_PlayChannel(WRONG_KEY, sounds[WRONG_KEY], 0);
        }
        void play_help_screen() {
            if (Mix_Playing(START_SCREEN)) {
                Mix_HaltChannel(START_SCREEN);
            }
            Mix_PlayChannel(HELP_SCREEN, sounds[HELP_SCREEN], 0);
        }
        void play_exit() {
            if (Mix_Playing(START_SCREEN)) {
                Mix_HaltChannel(START_SCREEN);
            }
            Mix_PlayChannel(EXIT, sounds[EXIT], 0);
            while (Mix_Playing(EXIT)) ;
        }
        void play_game_over() {
            Mix_PlayChannel(GAME_OVER, sounds[GAME_OVER], 0);
            while (Mix_Playing(GAME_OVER)) ;
        }
        void play_start_wave() {
            Mix_PlayChannel(START_WAVE, sounds[START_WAVE], 0);
            while (Mix_Playing(START_WAVE)) ;
        }
        void play_end_wave() {
            Mix_PlayChannel(END_WAVE, sounds[END_WAVE], 0);
            while (Mix_Playing(END_WAVE)) ;
        }
        void play_player_dead() {
            Mix_PlayChannel(PLAYER_DEAD, sounds[PLAYER_DEAD], 0);
            while (Mix_Playing(PLAYER_DEAD)) ;
        }
        void play_player_rebirth() {
            Mix_PlayChannel(PLAYER_REBIRTH, sounds[PLAYER_REBIRTH], 0);
            while (Mix_Playing(PLAYER_REBIRTH)) ;
        }
        void play_aliens_landed() {
            Mix_PlayChannel(ALIENS_LANDED, sounds[ALIENS_LANDED], 0);
            while (Mix_Playing(ALIENS_LANDED)) ;
        }
        void play_bonus() {
            if (!Mix_Playing(ACTIVE_BONUS)) {
                Mix_PlayChannel(ACTIVE_BONUS, sounds[ACTIVE_BONUS], 0);
            }
        }
        void halt_bonus() {
            if (Mix_Playing(ACTIVE_BONUS)) {
                Mix_HaltChannel(ACTIVE_BONUS);
            }
        }
        void play_bg(int alien_count) {
            if (alien_count > 4) {
                if (!Mix_Playing(BG)) {
                    Mix_PlayChannel(BG, sounds[BG], 0);
                }
            } else if (alien_count == 4){
                if (!Mix_Playing(BG_4)) {
                    Mix_PlayChannel(BG_4, sounds[BG_4], 0);
                }
            } else if (alien_count == 3){
                if (!Mix_Playing(BG_3)) {
                    Mix_PlayChannel(BG_3, sounds[BG_3], 0);
                }
            } else if (alien_count == 2){
                if (!Mix_Playing(BG_2)) {
                    Mix_PlayChannel(BG_2, sounds[BG_2], 0);
                }
            } else if (alien_count == 1){
                if (!Mix_Playing(BG_1)) {
                    Mix_PlayChannel(BG_1, sounds[BG_1], 0);
                }
            }
        }
        void halt_bg(int alien_count) {
            if (alien_count >= 4) {
                Mix_HaltChannel(BG);
            } else if (alien_count == 3) {
                Mix_HaltChannel(BG_4);
            } else if (alien_count == 2) {
                Mix_HaltChannel(BG_3);
            } else if (alien_count == 1) {
                Mix_HaltChannel(BG_2);
            } else if (alien_count == 0) {
                Mix_HaltChannel(BG_1);
            }
        }
        void halt_all_bg() {
            Mix_HaltChannel(BG);
            Mix_HaltChannel(BG_4);
            Mix_HaltChannel(BG_3);
            Mix_HaltChannel(BG_2);
            Mix_HaltChannel(BG_1);
        }

    };

}
#endif  //SOUND_H
