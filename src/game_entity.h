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

#include <assert.h>
#include <SDL/SDL.h>

#include "game.h"
#include "screen.h"

#define STATUS_ACTIVE   0
#define STATUS_ALIVE    1
#define STATUS_HIT      2  // used by Shot to avoid hitting more than one object

#define SET_BIT(var, bit)  var |= (1<<(bit))
#define CLR_BIT(var, bit)  var &= ~(1<<(bit))
#define TEST_BIT(var, bit)  ((var) & (1<<(bit)))

extern EventCounter event_counter;

enum GameEntityTypes {
    GAME_ENTITY_UNKNOWN,
    GAME_ENTITY_PLAYER,
    GAME_ENTITY_ALIEN,
    GAME_ENTITY_ALIEN2,
    GAME_ENTITY_ALIEN3,
    GAME_ENTITY_BONUS_SHIP,
    GAME_ENTITY_SMALL_BONUS_SHIP,
    GAME_ENTITY_SHOT,
    GAME_ENTITY_SHIELD_PIECE,
    GAME_ENTITY_EXPLOSION,
    GAME_ENTITY_SHIELD_GROUP,
    NUM_GAME_ENTITY_TYPES,
};

#define NUM_GAME_ENTITY_IMAGES     8

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
    private:
        static const int short_explosion = 50;
        static const int long_explosion = 200;
        static GameEntityTypeProperties type_properties[NUM_GAME_ENTITY_TYPES];

        static Game::Game* game;      // Common pointer to the current game.

        fixed x, y;   // location
        int dx, dy;   // velocity -- speed in pixels/sec and direction
        uint8_t status_bits;
        uint16_t frame_time_count; // control in place animation speed
        uint8_t position;   // used by Aliens to determine if and when to fire
        uint8_t fire_chance;

        SDL_Surface* images[NUM_GAME_ENTITY_IMAGES];
        int image_num;
        int type;

        GameEntityTypeProperties* properties;
    public:
        GameEntity() : type(GAME_ENTITY_UNKNOWN),
                       status_bits(0),
                       properties(&type_properties[GAME_ENTITY_UNKNOWN]) {}
        void init(int type, int x, int y, int dx, int dy, bool active);
        void movement(int16_t delta);
        static void set_type_property(int type,
                                      const GameEntityTypeProperties& prop) {
            type_properties[type] = prop;
        }
        void draw();
        void cleanup_draw();
        void erase();
        bool is_active() const {
            return TEST_BIT(status_bits, STATUS_ACTIVE) && is_alive();
        }
        void deactivate() { CLR_BIT(status_bits, STATUS_ACTIVE); }
        void activate() { SET_BIT(status_bits, STATUS_ACTIVE); }
        bool is_alive() const { return TEST_BIT(status_bits, STATUS_ALIVE); }
        void kill() { CLR_BIT(status_bits, STATUS_ALIVE); }
        void init_x(int init) { x = INT_TO_FIXED(init); }
        void init_y(int init) { y = INT_TO_FIXED(init); }
        int get_x() const { return x_int(); }
        int get_y() const { return y_int(); }
        int x_int() const { return FIXED_TO_INT(x); }
        int y_int() const { return FIXED_TO_INT(y); }
        int coll_w() const { return properties->coll_w; }
        int coll_h() const { return properties->coll_h; }
        int coll_x_offset() const { return properties->coll_x_offset; }
        int coll_y_offset() const { return properties->coll_y_offset; }
        bool collides_with(GameEntity* other);
        // can be used by classes with in-place animation
        void set_frame_duration(Uint32 dur) { properties->frame_duration = dur; }
        // Explosion
        void set_explosion(Uint32 dur) { activate(); properties->frame_duration = dur; }
        void duration(int16_t delta);
        // Alien
        void increase_x_speed(fixed increase) { dx = FIXED_TO_INT(dx * increase); }
        void set_x_velocity(int vel) { dx = vel; }
        void do_alien_logic() {  dx = -dx; y += INT_TO_FIXED(10); } // switch direction and move down the screen
        int get_pos() const { return position; }
        int get_fire_chance() const { return fire_chance; }
        // Shot
        void set_hit(bool hit) {
            if (hit)
                SET_BIT(status_bits, STATUS_HIT);
            else
                CLR_BIT(status_bits, STATUS_HIT);
        }
        bool is_hit() const { return TEST_BIT(status_bits, STATUS_HIT); }
        // collision handling
        void alien_shield_collision(GameEntity* other) { other->kill(); }
        void player_alien_collision(GameEntity* other);
        void player_shot_collision(GameEntity* other);
        void shot_alien_collision(GameEntity* other);
        void shot_shield_collision(GameEntity* other);
        void shot_shot_collision(GameEntity* other);
        void bonus_shot_collision(GameEntity* other);

        // Static member mutator.  Make sure to check that there is no existing
        // Game pointer being overwritten.
        static void set_game(Game::Game *game) {
            if (game)
                assert(GameEntity::game == NULL);
            GameEntity::game = game;
        }

        // Per-type functions.
        void Player_init(int x, int y, int dx, int dy, bool active);
        void Player_movement(int16_t delta);

        void Alien_init(int type, int x, int y, int dx, int dy, bool active, int pos, int chance);
        void Alien_movement(int16_t delta);

        void BonusShip_init(bool is_small, int x, int y, int dx, int dy, bool active);
        void BonusShip_movement(int16_t delta);

        void Shot_init(int x, int y, int dx, int dy, bool active);
        void Shot_movement(int16_t delta);

        void Explosion_init(int x, int y, int dx, int dy, bool active)
        {
            init(GAME_ENTITY_EXPLOSION, x, y, dx, dy, active);
            image = game->get_image("explosion.png");
        }

        void ShieldPiece_init(int x, int y, int dx, int dy, bool active)
        {
            init(GAME_ENTITY_SHIELD_PIECE, x, y, dx, dy, active);
            image = game->get_image("shield_piece.png");

            properties->coll_w = image->w;
            properties->coll_h = int (image->h * 0.9);
            properties->coll_x_offset = (image->w - properties->coll_w) / 2;
            properties->coll_y_offset = (image->h - properties->coll_h) / 2;
        }
    };

    // Retain these GameEntity subclass names so dependent code doesn't have to
    // be changed.
    typedef GameEntity Player;
    typedef GameEntity Alien;
    typedef GameEntity BonusShip;
    typedef GameEntity Shot;
    typedef GameEntity Explosion;
    typedef GameEntity ShieldPiece;
}
#endif  //GAME_ENTITY_H
