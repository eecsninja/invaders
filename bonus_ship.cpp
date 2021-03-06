/*
 bonus_ship.cpp
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

#define NUM_BONUS_SHIP_IMAGES     2

namespace GameEntities {

    void GameEntity::BonusShip_init(bool is_small, int x, int y, bool active)
    {
        init(is_small ? GAME_ENTITY_SMALL_BONUS_SHIP : GAME_ENTITY_BONUS_SHIP,
             0, x, y, active);
    }

    void GameEntity::BonusShip_movement(int16_t delta, int speed)
    {
        // control in place animation
        frame_time_count += delta;
        if (frame_time_count > properties()->frame_duration) {
            frame_time_count = 0;
            if (++image_num >= NUM_BONUS_SHIP_IMAGES) {
                image_num = 0;
            }
        }
        int dx = speed;
        x += INT_TO_FIXED(delta * dx) / 1000;
        if (dx > 0 && x_int() > screen_w) {
            deactivate();
        } else if (dx < 0 && x_int() < properties()->w) {
            deactivate();
        }
    }
}
