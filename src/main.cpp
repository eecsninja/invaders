/*
 main.cpp
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

#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "images.h"
#include "screen.h"
#include "system.h"

const char* datadir;

#ifdef __AVR__

#include "image_data.h"

using Graphics::ImageListEntry;

static const ImageListEntry image_list[] = {
    // The order here must match the order of corresponding GameEntity type
    // enumerations.
    ImageListEntry(ship_bmp_raw_data8, SHIP_BMP_RAW_DATA_SIZE),
    ImageListEntry(alien_1_bmp_raw_data8, ALIEN_1_BMP_RAW_DATA_SIZE),
    ImageListEntry(alien_2_bmp_raw_data8, ALIEN_2_BMP_RAW_DATA_SIZE),
    ImageListEntry(alien_3_bmp_raw_data8, ALIEN_3_BMP_RAW_DATA_SIZE),
    ImageListEntry(bonus_bmp_raw_data8, BONUS_BMP_RAW_DATA_SIZE),
    ImageListEntry(bonus_small_bmp_raw_data8, BONUS_SMALL_BMP_RAW_DATA_SIZE),
    ImageListEntry(shield_bmp_raw_data8, SHIELD_BMP_RAW_DATA_SIZE),
    ImageListEntry(shot_bmp_raw_data8, SHOT_BMP_RAW_DATA_SIZE),
    ImageListEntry(explosion_bmp_raw_data8, EXPLOSION_BMP_RAW_DATA_SIZE),
    ImageListEntry(0, 0),       // Terminate the list with NULL.
};

#else

// a list of image files are loaded into a map and keyed by filename
static const char* image_list[] = {
    "ship.png", "shot.png",
    "alien-1-1.png",  "alien-1-2.png",  "alien-1-3.png",  "alien-1-4.png",
    "alien-2-1.png",  "alien-2-2.png",  "alien-2-3.png",  "alien-2-4.png",
    "alien-3-1.png",  "alien-3-2.png",  "alien-3-3.png",  "alien-3-4.png",
    "bonus-1-1.png",  "bonus-1-2.png",  "wave_background.png", "background.png",
    "explosion.png", "shield_piece.png", "bonus-2-1.png", "bonus-2-2.png",
    "ui_header.png", "ui_points.png", NULL
};

#endif

int main(int argc, char* argv[])
{
    datadir = "data/";

    if (!System::init())
        return -1;

    Graphics::Images images;
    Graphics::Screen screen;

#if defined(__i386__)
    printf("This is a 32-bit x86 system.\n");
#elif defined(__amd64__) || defined(__x86_64__)
    printf("This is a 64-bit x86 system.\n");
#elif defined(__AVR__)
    printf("This is an AVR system.\n");
#else
    #error "Architecture type not supported."
#endif

    // Initialize video screen and image library.
    if (!screen.init())
        return -2;
    if (!images.load_images(image_list))
        return -3;
    screen.set_image_lib(&images);

    Game::Game game(&screen);
    game.game_control();

#ifdef __AVR__
    // Don't let the program exit on an embedded system, as there is no
    // operating system beyond the return statement.
    printf("End of program.\n");
    while (1);
#endif

    return 0;
}
