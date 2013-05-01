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
#include <map>
#include <vector>
#include <SDL/SDL.h>

#define dump(x) printf("%s %u: %s = %d\n", __func__, __LINE__, #x, (int)(x))
#define dumphex(x) printf("%s %u: %s = 0x%x\n", __func__, __LINE__, #x, (int)(x))

typedef int (*generator_fun)(void);   // Used for generating random values.

typedef int16_t fixed;    // Used for fixed point math on embedded systems.
#define FIXED_POINT_FACTOR                              16
// Convert to 32-bit int for multiplication, to avoid losing precision.
#define INT_TO_FIXED(x)           (((int32_t)x) * FIXED_POINT_FACTOR)
#define FIXED_TO_INT(x)           (x / FIXED_POINT_FACTOR)

namespace GameEntities {
    class GameEntity;
    class Alien;
    class ShieldPiece;
}

namespace Game {

    class Game {
        typedef GameEntities::GameEntity* GameEntityPtr;
        Sound::Sound sound;
        Ui::Ui ui;
        Ui::Status status;
        GameEntityPtr player, rbonus, sbonus, bonus;
        std::map<const char*, SDL_Surface*> image_cache;
        GameEntities::Alien* aliens;
        GameEntities::ShieldPiece* shields;
        std::vector<GameEntityPtr> player_shots, alien_shots, explosions;
        std::vector<int> direction, bonus_select, launch_delay;
        int alien_count, wave, player_life, alien_speed, alien_odd_range, num_alien_shots, num_entities_removed;
        int player_shot_counter, alien_shot_counter, explosion_counter;
        Uint32 last_shot, last_alien_shot, last_bonus_launch, last_loop_time, delta, score, dead_pause;
        Uint32 player_shot_delay, alien_shot_delay, bonus_launch_delay, next_free_guy;
        bool logic_this_loop, player_dead, wave_over, aliens_landed, sdl_quit_event;
        void free_guy_check();
        void init_aliens(generator_fun& gen, int speed);
        void pause();
        int none_active(const std::vector<GameEntityPtr>& vec);
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
        void free_entities();
    public:
        Game();
        ~Game();
        void explode(fixed x, fixed y, Uint32 duration);
        void game_control();
        SDL_Surface* get_image(const char* image);
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
