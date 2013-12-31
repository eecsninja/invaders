/*
 starfield.h
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

#ifndef STARFIELD_H
#define STARFIELD_H

#include <stdint.h>

namespace Graphics {
class Screen;
}

// Generates randomized starfield tiles and tilemap.
// screen:              Video controller.
// layer:               Tile layer to use.
// tile_width/height:   Tile dimensions.
// num_tiles:           Number of distinct starfield tiles to generate.
// num_stars:           Expected number of stars per tile.  Generated randomly,
//                        so actual number may deviate from this.
// min_brightness,
//    max_brightness:   Range of star brightness (0-255).
void generate_starfield(Graphics::Screen* screen,
                        uint8_t layer, uint8_t palette,
                        uint8_t tile_width, uint8_t tile_height,
                        uint8_t num_tiles, uint16_t num_stars,
                        uint8_t min_brightness, uint8_t max_brightness);

#endif  // STARFIELD_H
