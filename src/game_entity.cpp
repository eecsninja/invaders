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

namespace GameEntities {

    void GameEntity::erase()
    {
        blit* update;
        update = &blits[screen_updates++];
        update->img = wave_background;
        update->src_rect->x = (Sint16) x;
        update->src_rect->y = (Sint16) y;
        update->src_rect->w = image->w;
        update->src_rect->h = image->h;
        update->dst_rect->x = (Sint16) x;
        update->dst_rect->y = (Sint16) y;
        update->dst_rect->w = image->w;
        update->dst_rect->h = image->h;
    }
    void GameEntity::draw()
    {
        blit* update;
        update = &blits[screen_updates++];
        update->img = image;
        update->src_rect->x = 0;
        update->src_rect->y = 0;
        update->src_rect->w = image->w;
        update->src_rect->h = image->h;
        update->dst_rect->x = (Sint16) x;
        update->dst_rect->y = (Sint16) y;
        update->dst_rect->w = image->w;
        update->dst_rect->h = image->h;
    }
    void GameEntity::cleanup_draw()
    {
        src[0].x = src[0].y = 0;
        src[0].w = image->w;
        src[0].h = image->h;
        dst[0].x = (Sint16) x;
        dst[0].y = (Sint16) y;
        dst[0].w = image->w;
        dst[0].h = image->h;
        SDL_BlitSurface(image, &src[0], screen, &dst[0]);
    }
    bool GameEntity::collides_with(const boost::shared_ptr<GameEntity>& other)
    {
        if ( (this->y + this->coll_y_offset >= other->y + other->coll_y_offset + other->coll_h) ||
             (this->x + this->coll_x_offset >= other->x + other->coll_x_offset + other->coll_w) ||
             (other->y + other->coll_y_offset >= this->y + this->coll_y_offset + this->coll_h) ||
             (other->x + other->coll_x_offset >= this->x + this->coll_x_offset + this->coll_w) ) {
                return false;
        }
        return true;
    }
    void GameEntity::player_alien_collision(const boost::shared_ptr<GameEntity>& other)
    {
        game->explode(this->x, this->y, long_explosion);
        game->explode(other->x, other->y, short_explosion);
        this->active = false;
        game->remove_entity(other);
        game->msg_alien_player_collide();
    }
    void GameEntity::player_shot_collision(const boost::shared_ptr<GameEntity>& other)
    {
        game->explode(this->x, this->y, long_explosion);
        other->active = false;
        this->active = false;
        game->msg_player_dead();
    }
    void GameEntity::shot_alien_collision(const boost::shared_ptr<GameEntity>& other)
    {
        // prevent one bullet from destroying two entities
        if (hit) return;
        game->explode(other->x, other->y, short_explosion);
        this->active = false;
        game->msg_alien_killed(other->position, other->points);
        game->remove_entity(other);
        hit = true;
    }
    void GameEntity::shot_shield_collision(const boost::shared_ptr<GameEntity>& other)
    {
        if (hit) return;
        this->active = false;
        game->remove_entity(other);
        hit = true;
    }
    void GameEntity::shot_shot_collision(const boost::shared_ptr<GameEntity>& other)
    {
        // remove both shots
        this->active = false;
        other->active = false;
    }
    void GameEntity::bonus_shot_collision(const boost::shared_ptr<GameEntity>& other)
    {
        game->explode(this->x, this->y, long_explosion);
        this->active = false;
        other->active = false;
        game->msg_bonus_ship_destroyed(this->points);
    }
    void GameEntity::duration(Uint32 delta)
    {
        // control explosion duration
        frame_time_count += delta;
        if (frame_time_count > frame_duration) {
            frame_time_count = 0;
            active = false;
        }
    }

}
