/*
 game_entity_types.h
 Classic Invaders

 Copyright (c) 2010, Todd Steinackle, Simon que
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

#ifndef GAME_ENTITY_TYPES_H
#define GAME_ENTITY_TYPES_H

enum GameEntityTypes {
    GAME_ENTITY_PLAYER,
    GAME_ENTITY_ALIEN,
    GAME_ENTITY_ALIEN2,
    GAME_ENTITY_ALIEN3,
    GAME_ENTITY_BONUS_SHIP,
    GAME_ENTITY_SMALL_BONUS_SHIP,
    GAME_ENTITY_SHOT,
    GAME_ENTITY_SHIELD_PIECE,
    GAME_ENTITY_EXPLOSION,
    GAME_ENTITY_SHIELD_GROUP,
    GAME_ENTITY_UNKNOWN,
    NUM_GAME_ENTITY_TYPES,
};

#endif  // GAME_ENTITY_TYPES_H
