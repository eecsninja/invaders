/*
 alien.cpp
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

#include "alien.h"

namespace GameEntities {

    void Alien::movement(Uint32 delta)
    {
        // control in place animation
        frame_time_count += delta;
        if (frame_time_count > type_properties->frame_duration) {
            frame_time_count = 0;
            if (++image_num >= num_images) {
                image_num = 0;
            }
            image = images[image_num];
        }
        // bottom of the screen, game over
        if (y_int() > type_properties->bottom_limit) {
            game->msg_alien_landed();
        }
        // change direction and move down
        if (dx < 0 && x_int() < side_padding) {
            game->run_logic();
        }
        else if (dx > 0 && x_int() > type_properties->right_limit - side_padding) {
            game->run_logic();
        }

        x += delta * dx;
    }

    GameEntityTypeProperties Alien::alien_properties;
    GameEntityTypeProperties Alien2::alien2_properties;
    GameEntityTypeProperties Alien3::alien3_properties;
}
