/*
 game.h
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

#ifndef GAME_H
#define GAME_H

#include "ui.h"
#include "status.h"
#include "sound.h"
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <list>
#include <map>
#include <vector>
#include <SDL/SDL.h>

typedef boost::mt19937 base_generator_type;
typedef boost::variate_generator<base_generator_type&, boost::uniform_int<> > generator_fun;

namespace GameEntities {
    class GameEntity;
}

namespace Game {

    class Game {
        Sound::Sound sound;
        Ui::Ui ui;
        Ui::Status status;
        boost::shared_ptr<GameEntities::GameEntity> player, rbonus, sbonus, bonus;
        std::map<const char*, SDL_Surface*> image_cache;
        std::list<boost::shared_ptr<GameEntities::GameEntity> > aliens, dead_entities, shields;
        std::vector<boost::shared_ptr<GameEntities::GameEntity> > player_shots, alien_shots, explosions;
        std::vector<int> direction, bonus_select, launch_delay;
        int alien_count, wave, player_life, alien_speed, alien_odd_range, num_alien_shots, num_entities_removed;
        int player_shot_counter, alien_shot_counter, explosion_counter;
        Uint32 last_shot, last_alien_shot, last_bonus_launch, last_loop_time, delta, score, dead_pause;
        Uint32 player_shot_delay, alien_shot_delay, bonus_launch_delay, next_free_guy;
        bool logic_this_loop, player_dead, wave_over, aliens_landed, sdl_quit_event;
        void free_guy_check();
        void init_aliens(generator_fun& gen, int speed);
        void pause();
        int none_active(const std::vector<boost::shared_ptr<GameEntities::GameEntity> >& vec);
        void launch_bonus_ship();
        void alien_fire();
        void load_images();
        void free_images();
        void fire_shot();
        void init_wave();
        void factory();
        void game_loop();
        void wave_cleanup();
        void player_rebirth();
    public:
        Game();
        ~Game();
        void explode(double x, double y, Uint32 duration);
        void game_control();
        SDL_Surface* get_image(const char* image);
        void remove_entity(const boost::shared_ptr<GameEntities::GameEntity>& e) {
            dead_entities.push_front(e);
            ++num_entities_removed;
        }
        void msg_player_dead();
        void msg_alien_landed();
        void msg_alien_killed(int pos, int points);
        void msg_alien_player_collide();
        void msg_bonus_ship_destroyed(int bonus);
        void run_logic() { logic_this_loop = true; }
        void set_video_mode(int fullscreen);
    };

}
#endif  //GAME_H
