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

#ifndef SHIELDS_H
#define SHIELDS_H

#include <stdint.h>

#include "game_defs.h"

namespace Game {

    struct ShieldPiece;

    // For drawing a shield group.
    class ShieldGroupTiles {
      private:
        // For drawing two shield rows.
        struct ShieldDoubleRow {
            // Contains tile map data.  Every element of the array is a 16x16
            // tile that represents a 2x2 shield cluster.  So the width is half
            // the width of a shield group.
            uint16_t map_buffer[SHIELD_GROUP_WIDTH / 2];

            // Set this flag when the buffer is updated, and the contents need
            // to be copied to the video controller.
            uint8_t dirty:1;
        } rows[SHIELD_GROUP_HEIGHT / 2];

      public:
        ShieldGroupTiles();

        // Initialize the tile map buffer based on a given array of shields.
        void init(const ShieldPiece* shields);

        // Updates the corresponding buffer tile bit for a shield piece.
        void update_shield_piece(const ShieldPiece& shield);
    };

}  // namespace Game

#endif  // SHIELDS_H
