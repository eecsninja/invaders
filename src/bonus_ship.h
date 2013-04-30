/*
 bonus_ship.h
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

#ifndef BONUSSHIP_H
#define BONUSSHIP_H

#include "game_entity.h"

namespace GameEntities {

    class BonusShip : public GameEntity {
    protected:
        static const int num_images = 2;
        static GameEntityTypeProperties bonus_ship_properties;
        SDL_Surface* images[num_images];
        int image_num;
    public:
        BonusShip(int x, int y, int dx, int dy, bool active, Game::Game* game)
            : GameEntity(x, y, dx, dy, active, game), image_num(0)
        {
            image = images[0] = game->get_image("bonus-1-1.png");
            images[1] = game->get_image("bonus-1-2.png");

            type_properties = &bonus_ship_properties;
            type_properties->frame_duration = 55;
            type_properties->points = 1000;
            type_properties->coll_w = int (image->w * 0.9);
            type_properties->coll_h = image->h;
            type_properties->coll_x_offset = (image->w - type_properties->coll_w) / 2;
            type_properties->coll_y_offset = (image->h - type_properties->coll_h) / 2;
        }
        void movement(Uint32 delta);
    };

    class SmallBonusShip : public BonusShip {
        static GameEntityTypeProperties small_bonus_ship_properties;
    public:
        SmallBonusShip(int x, int y, int dx, int dy, bool active, Game::Game* game)
            : BonusShip(x, y, dx, dy, active, game)
        {
            image = images[0] = game->get_image("bonus-2-1.png");
            images[1] = game->get_image("bonus-2-2.png");

            small_bonus_ship_properties = bonus_ship_properties;
            type_properties = &small_bonus_ship_properties;
            type_properties->points = 5000;
        }
    };

}
#endif  //BONUSSHIP_H
