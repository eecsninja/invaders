/*
 shields.h
 Classic Invaders

 Copyright (c) 2013, Todd Steinackle, Simon Que
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

#include "shields.h"

#include <string.h>

#include "game.h"

namespace Game {

    // Clear everything when this is created.
    ShieldGroupTiles::ShieldGroupTiles() {
        for (uint8_t i = 0; i < SHIELD_GROUP_HEIGHT / 2; ++i) {
            memset(rows[i].map_buffer, 0, sizeof(rows[i].map_buffer));
            rows[i].dirty = 0;
        }
    }

    void ShieldGroupTiles::init(const ShieldPiece* shields) {
        for (int i = 0; i < SHIELD_GROUP_WIDTH * SHIELD_GROUP_HEIGHT; ++i)
            update_shield_piece(shields[i]);
    }

    void ShieldGroupTiles::update_shield_piece(const ShieldPiece& shield) {
        uint8_t x = shield.x;
        uint8_t y = shield.y;

        // Since each tile is 2x2 shields, divide the shield x and y values by 2
        // to get the tile row and column.
        ShieldDoubleRow& row = rows[y / 2];

        // Determine the bit within the tile value that corresponds to the
        // current shield piece being updated:
        //           +-----+-----+
        //           |bit 0|bit 1|
        //           +-----+-----+
        //           |bit 2|bit 3|
        //           +-----+-----+
        uint8_t bit_offset = (x % 2) + (y % 2) * 2;

        // Set or clear the bit depending on the shield state.
        uint16_t& tile = row.map_buffer[x / 2];
        if (shield.intact)
            tile |= (1 << bit_offset);
        else
            tile &= ~(1 << bit_offset);

        row.dirty = true;   // Indicate that the row needs to be redrawn.
    }

}  // namespace Game
