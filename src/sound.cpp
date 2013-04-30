/*
 sound.cpp
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

#include "sound.h"
#include <iostream>
#include <sstream>
#include <boost/assign/list_of.hpp>

extern std::string datadir;

namespace Sound {

    Sound::Sound()
    {
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 512) < 0) {
            std::cerr << "Unable to set audio: " << SDL_GetError() << '\n';
            throw "SDL_Mixer_Error: sound object not created.";
        }

        std::vector<const char*> sound_files = boost::assign::list_of("shot.ogg") ("explosion.ogg")
            ("alien_death.ogg")  ("free_guy.ogg")  ("game_over.ogg")  ("start_wave.ogg")
            ("big_bonus.ogg")  ("small_bonus.ogg")  ("shot_collision.ogg")  ("active_bonus.ogg")
            ("start_screen.ogg")  ("player_dead.ogg")  ("player_rebirth.ogg")  ("aliens_landed.ogg")
            ("end_wave.ogg")  ("menu_select.ogg")  ("zap_scores.ogg") ("new_high_score.ogg")
            ("about_screen.ogg") ("wrong_key.ogg") ("help_screen.ogg") ("exit.ogg") ("bg.ogg")
            ("bg_4.ogg") ("bg_3.ogg") ("bg_2.ogg") ("bg_1.ogg")
        ;

        typedef std::vector<const char*>::const_iterator Iter;
        for (Iter it = sound_files.begin(); it != sound_files.end(); ++it) {
            std::string path = datadir + *it;
            sounds.push_back(Mix_LoadWAV(path.c_str()));
            if (sounds.back() == NULL) {
                free_sounds();
                Mix_CloseAudio();
                std::ostringstream s;
                s << "Unable to load sound file: " << *it << '\n';
                throw s.str();
            }
        }

        Mix_AllocateChannels(sounds.size());
        Mix_VolumeChunk(sounds[SHOT], 15);
        Mix_VolumeChunk(sounds[EXPLOSION], 100);
        Mix_VolumeChunk(sounds[ALIEN_DEATH], 10);
        Mix_VolumeChunk(sounds[FREE_GUY], 100);
        Mix_VolumeChunk(sounds[GAME_OVER], 100);
        Mix_VolumeChunk(sounds[START_WAVE], 100);
        Mix_VolumeChunk(sounds[BIG_BONUS], 95);
        Mix_VolumeChunk(sounds[SMALL_BONUS], 95);
        Mix_VolumeChunk(sounds[SHOT_COLLISION], 35);
        Mix_VolumeChunk(sounds[ACTIVE_BONUS], 10);
        Mix_VolumeChunk(sounds[START_SCREEN], 100);
        Mix_VolumeChunk(sounds[PLAYER_DEAD], 100);
        Mix_VolumeChunk(sounds[PLAYER_REBIRTH], 100);
        Mix_VolumeChunk(sounds[ALIENS_LANDED], 100);
        Mix_VolumeChunk(sounds[END_WAVE], 100);
        Mix_VolumeChunk(sounds[MENU_SELECT], 90);
        Mix_VolumeChunk(sounds[ZAP_SCORES], 65);
        Mix_VolumeChunk(sounds[NEW_HIGH_SCORE], 100);
        Mix_VolumeChunk(sounds[ABOUT_SCREEN], 100);
        Mix_VolumeChunk(sounds[WRONG_KEY], 90);
        Mix_VolumeChunk(sounds[HELP_SCREEN], 100);
        Mix_VolumeChunk(sounds[EXIT], 100);
        Mix_VolumeChunk(sounds[BG], 100);
        Mix_VolumeChunk(sounds[BG_4], 100);
        Mix_VolumeChunk(sounds[BG_3], 100);
        Mix_VolumeChunk(sounds[BG_2], 100);
        Mix_VolumeChunk(sounds[BG_1], 100);
    }
    Sound::~Sound()
    {
        free_sounds();
        Mix_CloseAudio();
    }
    void Sound::free_sounds()
    {
        typedef std::vector<Mix_Chunk*>::iterator vIter;
        for (vIter it = sounds.begin(); it != sounds.end(); ++it) {
            Mix_FreeChunk(*it);
        }
    }

}
