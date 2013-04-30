/*
 alien.h
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

#ifndef ALIEN_H
#define ALIEN_H

#include "game_entity.h"

namespace GameEntities {

    class Alien : public GameEntity {
    protected:
        static GameEntityTypeProperties alien_properties;

        static const int num_images = 6;
        SDL_Surface* images[num_images];
        int image_num;
    public:
        Alien(int x, int y, int dx, int dy, bool active, Game::Game* game, int pos, int chance)
            : GameEntity(x, y, dx, dy, active, game), image_num(0)
        {
            image = images[0] = game->get_image("alien-1-1.png");
            images[1] = game->get_image("alien-1-2.png");
            images[2] = game->get_image("alien-1-3.png");
            images[3] = game->get_image("alien-1-4.png");
            images[4] = game->get_image("alien-1-3.png");
            images[5] = game->get_image("alien-1-2.png");

            position = pos;
            fire_chance = chance;

            type_properties = &alien_properties;
            type_properties->points = 25;
            type_properties->frame_duration = 225;
            type_properties->right_limit = screen_w - image->w;
            type_properties->bottom_limit = 530;
            type_properties->coll_w = image->w;
            type_properties->coll_h = int (image->h * 0.8);
            type_properties->coll_x_offset = (image->w - type_properties->coll_w) / 2;
            type_properties->coll_y_offset = (image->h - type_properties->coll_h) / 2;
        }
        void movement(int16_t delta);
    };

    class Alien2 : public Alien {
        static GameEntityTypeProperties alien2_properties;
    public:
        Alien2(int x, int y, int dx, int dy, bool active, Game::Game* game, int pos, int chance)
            : Alien(x, y, dx, dy, active, game, pos, chance)
        {
            image = images[0] = game->get_image("alien-2-1.png");
            images[1] = game->get_image("alien-2-2.png");
            images[2] = game->get_image("alien-2-3.png");
            images[3] = game->get_image("alien-2-4.png");
            images[4] = game->get_image("alien-2-3.png");
            images[5] = game->get_image("alien-2-2.png");

            alien2_properties = alien_properties;
            type_properties = &alien2_properties;
            type_properties->points = 50;
        }
    };

    class Alien3 : public Alien {
        static GameEntityTypeProperties alien3_properties;
    public:
        Alien3(int x, int y, int dx, int dy, bool active, Game::Game* game, int pos, int chance)
            : Alien(x, y, dx, dy, active, game, pos, chance)
        {
            image = images[0] = game->get_image("alien-3-1.png");
            images[1] = game->get_image("alien-3-2.png");
            images[2] = game->get_image("alien-3-3.png");
            images[3] = game->get_image("alien-3-4.png");
            images[4] = game->get_image("alien-3-3.png");
            images[5] = game->get_image("alien-3-2.png");

            alien3_properties = alien_properties;
            type_properties = &alien3_properties;
            type_properties->points = 100;
            type_properties->coll_w = int (image->w * 0.8);
            type_properties->coll_x_offset = (image->w - type_properties->coll_w) / 2;
        }
    };

}
#endif  //ALIEN_H
