/*
 game_entity.h
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

#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include "game.h"
#include <SDL/SDL.h>
#include "screen.h"

#define FIXED_POINT_FACTOR                            1024
#define INT_TO_FIXED(x)           (x * FIXED_POINT_FACTOR)
#define FIXED_TO_INT(x)           (x / FIXED_POINT_FACTOR)

namespace GameEntities {

    // When all instances of a class of GameEntity have the same properties,
    // use this struct to store the common property values and save memory.
    struct GameEntityTypeProperties {
        uint16_t frame_duration;  // How much time before going to next frame.
        int points; // point value of individual objects
        // heights, widths, and coords used for reduced bounding box collision detection
        int coll_w, coll_h, coll_x_offset, coll_y_offset;

        int right_limit, bottom_limit;
    };

    class GameEntity {
        static const int short_explosion = 50;
        static const int long_explosion = 200;
    protected:
        fixed x, y;   // location
        int dx, dy;   // velocity -- speed in pixels/sec and direction
        uint8_t active;
        Game::Game* game;
        SDL_Surface* image;
        uint16_t frame_time_count; // control in place animation speed
        uint16_t position;   // used by Aliens to determine if and when to fire
        uint8_t fire_chance;
        uint8_t hit; // used by Shot to avoid hitting more than one object

        GameEntityTypeProperties* type_properties;
    public:
        GameEntity(int x, int y, int dx, int dy, bool active, Game::Game* game)
            : x(INT_TO_FIXED(x)), y(INT_TO_FIXED(y)), dx(dx), dy(dy), active(active), game(game),
            frame_time_count(0), hit(false) { }
        // a virtual destructor is important
        virtual ~GameEntity() { }
        virtual void movement(Uint32 delta) { delta = 0; }
        void draw();
        void cleanup_draw();
        void erase();
        bool is_active() const { return active; }
        void deactivate() { active = false; }
        void activate() { active = true; }
        void init_x(int init) { x = INT_TO_FIXED(init); }
        void init_y(int init) { y = INT_TO_FIXED(init); }
        int get_x() const { return x_int(); }
        int get_y() const { return y_int(); }
        int x_int() const { return FIXED_TO_INT(x); }
        int y_int() const { return FIXED_TO_INT(y); }
        int coll_w() const { return type_properties->coll_w; }
        int coll_h() const { return type_properties->coll_h; }
        int coll_x_offset() const { return type_properties->coll_x_offset; }
        int coll_y_offset() const { return type_properties->coll_y_offset; }
        bool collides_with(GameEntity* other);
        // can be used by classes with in-place animation
        void set_frame_duration(Uint32 dur) { type_properties->frame_duration = dur; }
        // Explosion
        void set_explosion(Uint32 dur) { active = true; type_properties->frame_duration = dur; }
        void duration(Uint32 delta);
        // Alien
        void increase_x_speed(fixed increase) { dx = FIXED_TO_INT(dx * increase); }
        void set_x_velocity(int vel) { dx = vel; }
        void do_alien_logic() {  dx = -dx; y += INT_TO_FIXED(10); } // switch direction and move down the screen
        int get_pos() const { return position; }
        int get_fire_chance() const { return fire_chance; }
        // Shot
        void set_hit(bool h) { hit = h; }
        // collision handling
        void alien_shield_collision(GameEntity* other) { game->remove_entity(other); }
        void player_alien_collision(GameEntity* other);
        void player_shot_collision(GameEntity* other);
        void shot_alien_collision(GameEntity* other);
        void shot_shield_collision(GameEntity* other);
        void shot_shot_collision(GameEntity* other);
        void bonus_shot_collision(GameEntity* other);
    };

}
#endif  //GAME_ENTITY_H
