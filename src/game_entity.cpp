/*
 game_entity.cpp
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

#include "game_entity.h"

#include "screen.h"

#define short_explosion       50
#define long_explosion       200

namespace GameEntities {

    void GameEntity::init(int type, int x, int y, int dx, int dy, bool active)
    {
        this->type = type;
        this->x = INT_TO_FIXED(x);
        this->y = INT_TO_FIXED(y);
        this->dx = dx;
        this->dy = dy;
        this->status_bits = (active ? (1<<STATUS_ACTIVE) : 0) | (1<<STATUS_ALIVE);
        this->frame_time_count = 0;
        this->image_num = 0;
    }

    void GameEntity::draw()
    {
        video->schedule_blit(properties()->images[image_num], x_int(), y_int());
    }
    void GameEntity::cleanup_draw()
    {
        draw();
    }
    bool GameEntity::collides_with(GameEntity* other)
    {
#ifdef EVENT_COUNTER
        event_counter.do_collision_check();
#endif
        if ( (this->y_int() + this->coll_y_offset() >= other->y_int() + other->coll_y_offset() + other->coll_h()) ||
             (this->x_int() + this->coll_x_offset() >= other->x_int() + other->coll_x_offset() + other->coll_w()) ||
             (other->y_int() + other->coll_y_offset() >= this->y_int() + this->coll_y_offset() + this->coll_h()) ||
             (other->x_int() + other->coll_x_offset() >= this->x_int() + this->coll_x_offset() + this->coll_w()) ) {
                return false;
        }
        return true;
    }
    void GameEntity::player_alien_collision(GameEntity* other)
    {
        game->explode(this->x_int(), this->y_int(), long_explosion);
        game->explode(other->x_int(), other->y_int(), short_explosion);
        this->deactivate();
        other->kill();
        game->msg_alien_player_collide();
    }
    void GameEntity::player_shot_collision(GameEntity* other)
    {
        game->explode(this->x_int(), this->y_int(), long_explosion);
        other->deactivate();
        this->deactivate();
        game->msg_player_dead();
    }
    void GameEntity::shot_alien_collision(GameEntity* other)
    {
        // prevent one bullet from destroying two entities
        if (is_hit())
            return;
        game->explode(other->x_int(), other->y_int(), short_explosion);
        this->deactivate();
        game->msg_alien_killed(other->position, other->properties()->points);
        other->kill();
        set_hit(true);
    }
    void GameEntity::shot_shield_collision(GameEntity* other)
    {
        if (is_hit())
            return;
        this->deactivate();
        other->kill();
        set_hit(true);
    }
    void GameEntity::shot_shot_collision(GameEntity* other)
    {
        // remove both shots
        this->deactivate();
        other->deactivate();
    }
    void GameEntity::bonus_shot_collision(GameEntity* other)
    {
        game->explode(this->x_int(), this->y_int(), long_explosion);
        this->deactivate();
        other->deactivate();
        game->msg_bonus_ship_destroyed(this->properties()->points);
    }
    void GameEntity::duration(int16_t delta)
    {
        // control explosion duration
        frame_time_count += delta;
        if (frame_time_count > properties()->frame_duration) {
            frame_time_count = 0;
            deactivate();
        }
#ifdef EVENT_COUNTER
        event_counter.do_duration_call();
#endif
    }
    void GameEntity::movement(int16_t delta)
    {
        switch(type) {
        case GAME_ENTITY_PLAYER:
            Player_movement(delta);
            break;
        case GAME_ENTITY_ALIEN:
        case GAME_ENTITY_ALIEN2:
        case GAME_ENTITY_ALIEN3:
            Alien_movement(delta);
            break;
        case GAME_ENTITY_BONUS_SHIP:
        case GAME_ENTITY_SMALL_BONUS_SHIP:
            BonusShip_movement(delta);
            break;
        case GAME_ENTITY_SHOT:
            Shot_movement(delta);
            break;
        case GAME_ENTITY_UNKNOWN:
        case GAME_ENTITY_SHIELD_PIECE:
        case GAME_ENTITY_EXPLOSION:
        case GAME_ENTITY_SHIELD_GROUP:
        default:
            break;
        }
#ifdef EVENT_COUNTER
        event_counter.do_movement_call();
#endif
    }

    void GameEntity::Explosion_init(int x, int y, int dx, int dy, bool active)
    {
        init(GAME_ENTITY_EXPLOSION, x, y, dx, dy, active);
    }

    void GameEntity::ShieldPiece_init(int x, int y, int dx, int dy, bool active)
    {
        init(GAME_ENTITY_SHIELD_PIECE, x, y, dx, dy, active);
    }

    Game::Game* GameEntity::game = NULL;
    Graphics::Video* GameEntity::video = NULL;
    GameEntityTypeProperties GameEntity::type_properties[NUM_GAME_ENTITY_TYPES];
}
