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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fixed_point.h"
#include "screen.h"
#include "sound.h"
#include "status.h"
#include "system.h"
#include "ui.h"

#define dump(x) printf("%s %u: %s = %d\n", __func__, __LINE__, #x, (int)(x))
#define dumphex(x) printf("%s %u: %s = 0x%x\n", __func__, __LINE__, #x, (int)(x))

namespace GameEntities {
    class GameEntity;
    typedef GameEntity Alien;
    typedef GameEntity Explosion;
    typedef GameEntity Shot;
}

#define MAX_NUM_IMAGES    32

namespace Game {

    int get_image_index(const char* filename);

    struct ShieldPiece {
        uint8_t x:3;        // x-index of shield piece in shield group (0-5)
        uint8_t y:2;        // x-index of shield piece in shield group (0-3)
        uint8_t group:2;    // Shield group number (0-2)
        bool intact:1;   // Status of shield.
    };

    struct ShieldGroupTiles;

    class Game {
        typedef GameEntities::GameEntity* GameEntityPtr;
        // Sound::Sound sound;
        // Ui::Ui ui;
        // Ui::Status status;
        Graphics::Screen& screen;
        GameEntityPtr player, rbonus, sbonus, bonus;
        GameEntities::Alien* aliens;
        ShieldPiece* shields;  // Points to an array of shield states.
        GameEntities::Shot* player_shots, *alien_shots;
        GameEntities::Explosion* explosions;
        // For coarse collision detection with multiple shield pieces.
        // Greatly reduces the number of shield collision checks when there's
        // no collision.
        GameEntities::GameEntity* shield_groups;
        // For drawing shield groups.
        ShieldGroupTiles* shield_group_tiles;
        int* direction;
        int* bonus_select;
        int* launch_delay;
        int alien_count, wave, player_life, alien_odd_range, num_alien_shots;
        int current_player_speed, current_alien_speed, current_bonus_speed;
        int player_shot_counter, alien_shot_counter, explosion_counter;
        uint32_t last_shot, last_alien_shot, last_bonus_launch, last_loop_time, delta, score, dead_pause;
        uint32_t player_shot_delay, alien_shot_delay, bonus_launch_delay, next_free_guy;
        bool logic_this_loop, player_dead, wave_over, aliens_landed;
        void free_guy_check();
        void init_aliens(int rand_max);
        void pause();
        bool collides_with_shield_group(GameEntities::GameEntity* object);
        bool no_player_shots_active();
        bool no_alien_shots_active();
        bool no_explosions_active();
        void launch_bonus_ship();
        void alien_fire();
        void fire_shot();
        void init_wave();
        void factory();
        void game_loop();
        void wave_cleanup();
        void player_rebirth();
    public:
        Game(Graphics::Screen* screen);
        ~Game();
        void explode(fixed x, fixed y, uint32_t duration);
        void game_control();
        void msg_player_dead();
        void msg_alien_landed();
        void msg_alien_killed(int index, int points);
        void msg_alien_player_collide();
        void msg_bonus_ship_destroyed(int bonus);
        void run_logic() { logic_this_loop = true; }
    };

}
#endif  //GAME_H
