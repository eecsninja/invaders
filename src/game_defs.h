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

#define ALIEN_BASE_X         40
#define ALIEN_BASE_Y         36
#define ALIEN_STEP_X         20
#define ALIEN_STEP_Y         14
#define ALIEN_Y_MOVEMENT      4

#define ALIEN_SPEED_BOOST            (FIXED_POINT_FACTOR_32 * 1.027)
#define ALIEN_SPEED_BOOST_EXTRA      (FIXED_POINT_FACTOR_32 * 1.15)

// For generating shield pieces.
#define NUM_SHIELD_GROUPS            3
#define SHIELD_GROUP_WIDTH           6
#define SHIELD_GROUP_HEIGHT          4
#define NUM_SHIELDS_PER_GROUP   (SHIELD_GROUP_WIDTH * SHIELD_GROUP_HEIGHT)
#define NUM_SHIELDS             (NUM_SHIELD_GROUPS * NUM_SHIELDS_PER_GROUP)

#define SHIELD_X_OFFSET             40
#define SHIELD_Y_OFFSET            166
#define SHIELD_GROUP_X_SPACING      96

// Image dimensions scaled for embedded port.
#define PLAYER_WIDTH             18
#define PLAYER_HEIGHT            10
#define ALIEN_WIDTH              18
#define ALIEN_HEIGHT             12
#define BONUS_SHIP_WIDTH         24
#define BONUS_SHIP_HEIGHT        11
#define SMALL_BONUS_SHIP_WIDTH   16
#define SMALL_BONUS_SHIP_HEIGHT   7
#define SHOT_WIDTH                3
#define SHOT_HEIGHT               8
#define SHIELD_PIECE_SIZE         8

// These sprite dimensions are powers of two that contain the above image
// size definitions.
// TODO: use a function or macro to compute these from the image sizes.
#define PLAYER_SPRITE_WIDTH                32
#define PLAYER_SPRITE_HEIGHT               16
#define ALIEN_SPRITE_WIDTH                 32
#define ALIEN_SPRITE_HEIGHT                16
#define BONUS_SHIP_SPRITE_WIDTH            32
#define BONUS_SHIP_SPRITE_HEIGHT           16
#define SMALL_BONUS_SHIP_SPRITE_WIDTH      16
#define SMALL_BONUS_SHIP_SPRITE_HEIGHT      8
#define SHOT_SPRITE_WIDTH                   8
#define SHOT_SPRITE_HEIGHT                  8
#define EXPLOSION_SPRITE_SIZE              16

// Position definitions.
#define PLAYER_BOTTOM_GAP                          14
#define ALIEN_SHOT_GAP                              2
#define BONUS_TOP                                  24

#define player_center         ((screen_w - PLAYER_WIDTH) / 2)
#define player_top            (screen_h - (PLAYER_HEIGHT + PLAYER_BOTTOM_GAP))
#define player_init_x_shot_pos        ((PLAYER_WIDTH - SHOT_WIDTH) / 2)
#define player_init_y_shot_pos                SHOT_HEIGHT
#define alien_init_x_shot_pos         ((ALIEN_WIDTH - SHOT_WIDTH)  / 2)
#define alien_init_y_shot_pos         (ALIEN_HEIGHT - ALIEN_SHOT_GAP)

// Speed definitions in pixels/sec.
#define player_speed                              120
#define bonus_speed                                80
#define shot_speed                               -120
#define alien_shot_speed                           80
#define ALIEN_BASE_SPEED                           32
#define ALIEN_WAVE_SPEED_INCREASE                   2
#define ALIEN_HIGH_WAVE_SPEED_INCREASE              1

// Initial bonus ship delay in ms.
#define base_launch_delay                            6000

// Number of points to get an extra life.
#define free_guy_val                                50000

// Static array sizes.
#define random_list_len                                30
#define num_player_shots                                9
#define num_explosions                                  5
#define MAX_NUM_ALIEN_SHOTS                            32

#define NUM_STARFIELD_TILES                            16
#define STARFIELD_DENSITY                               8
#define STARFIELD_SPEED                                40

#endif  // GAME_DEFS_H
