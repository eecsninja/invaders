/*
 game_defs.h
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

#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include "screen.h"

// The formation of aliens.
#define ALIEN_ARRAY_WIDTH    12
#define ALIEN_ARRAY_HEIGHT    5
#define NUM_ALIENS      (ALIEN_ARRAY_WIDTH * ALIEN_ARRAY_HEIGHT)
#ifdef __AVR__
    #define ALIEN_BASE_X         40
    #define ALIEN_BASE_Y         36
    #define ALIEN_STEP_X         20
    #define ALIEN_STEP_Y         14
#else
    #define ALIEN_BASE_X        100
    #define ALIEN_BASE_Y         90
    #define ALIEN_STEP_X         50
    #define ALIEN_STEP_Y         35
#endif

#define ALIEN_SPEED_BOOST            (FIXED_POINT_FACTOR * 1.027)
#define ALIEN_SPEED_BOOST_EXTRA      (FIXED_POINT_FACTOR * 1.15)

// For generating shield pieces.
#define NUM_SHIELD_GROUPS            3
#define SHIELD_GROUP_WIDTH           6
#define SHIELD_GROUP_HEIGHT          4
#define NUM_SHIELDS   (NUM_SHIELD_GROUPS * SHIELD_GROUP_WIDTH * SHIELD_GROUP_HEIGHT)

#ifdef __AVR__
    #define SHIELD_X_OFFSET             44
    #define SHIELD_Y_OFFSET            166
    #define SHIELD_GROUP_X_SPACING      92
#else
    #define SHIELD_X_OFFSET            110
    #define SHIELD_Y_OFFSET            415
    #define SHIELD_GROUP_X_SPACING     230
#endif


#ifdef __AVR__
    // Image dimensions scaled for embedded port.
    #define PLAYER_WIDTH             18
    #define PLAYER_HEIGHT            10
    #define ALIEN_WIDTH              18
    #define ALIEN_HEIGHT             12
    #define BONUS_SHIP_WIDTH         24
    #define BONUS_SHIP_HEIGHT        11
    #define SMALL_BONUS_SHIP_WIDTH   18
    #define SMALL_BONUS_SHIP_HEIGHT   8
    #define SHOT_WIDTH                3
    #define SHOT_HEIGHT               8
    #define SHIELD_PIECE_SIZE         8
#else
    // Taken from original image file dimensions.
    #define PLAYER_WIDTH             43
    #define PLAYER_HEIGHT            25
    #define ALIEN_WIDTH              45
    #define ALIEN_HEIGHT             30
    #define BONUS_SHIP_WIDTH         60
    #define BONUS_SHIP_HEIGHT        26
    #define SMALL_BONUS_SHIP_WIDTH   45
    #define SMALL_BONUS_SHIP_HEIGHT  20
    #define SHOT_WIDTH                5
    #define SHOT_HEIGHT              16
    #define SHIELD_PIECE_SIZE        20
#endif  // defined (__AVR__)

// Position definitions.
#ifdef __AVR__
    #define PLAYER_BOTTOM_GAP                          14
    #define ALIEN_SHOT_GAP                              2
#else
    #define PLAYER_BOTTOM_GAP                          35
    #define ALIEN_SHOT_GAP                              5
#endif
#define player_center         ((screen_w - PLAYER_WIDTH) / 2)
#define player_top            (screen_h - (PLAYER_HEIGHT + PLAYER_BOTTOM_GAP))
#define player_init_x_shot_pos        ((PLAYER_WIDTH - SHOT_WIDTH) / 2)
#define player_init_y_shot_pos                SHOT_HEIGHT
#define alien_init_x_shot_pos         ((ALIEN_WIDTH - SHOT_WIDTH)  / 2)
#define alien_init_y_shot_pos         (ALIEN_HEIGHT - ALIEN_SHOT_GAP)

// Speed definitions in pixels/sec.
#ifdef __AVR__
    #define player_speed                              120
    #define bonus_speed                                80
    #define shot_speed                               -120
    #define alien_shot_speed                           80
#else
    #define player_speed                              300
    #define bonus_speed                               200
    #define shot_speed                               -300
    #define alien_shot_speed                          200
#endif  // defined (__AVR__)

// Initial bonus ship delay in ms.
#define base_launch_delay                            6000

// Number of points to get an extra life.
#define free_guy_val                                50000

// Static array sizes.
#define random_list_len                                30
#define num_player_shots                                9
#define num_explosions                                  5
#define MAX_NUM_ALIEN_SHOTS                            32

#endif  // GAME_DEFS_H