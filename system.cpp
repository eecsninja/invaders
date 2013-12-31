/*
 system.cpp
 Classic Invaders

 Copyright (c) 2013, Simon Que
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

#include "system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Arduino.h>
#include <DuinoCube.h>

namespace System {

    bool init() {
        return true;
    }

    KeyState get_key_state() {
        KeyState key_state;
        memset(&key_state, 0, sizeof(key_state));

        GamepadState gamepad = DC.Gamepad.readGamepad();
        if (gamepad.buttons & (1 << GAMEPAD_BUTTON_4))
            key_state.quit = 1;
        if (gamepad.buttons & (1 << GAMEPAD_BUTTON_2))
            key_state.pause = 1;
        if (gamepad.x == 0)
            key_state.left = 1;
        if (gamepad.x == UINT8_MAX)
            key_state.right = 1;
        if (gamepad.buttons & (1 << GAMEPAD_BUTTON_1))
            key_state.fire = 1;

        return key_state;
    }

    // This is just a wrapper around SDL_GetTicks.  As part of the embedded port,
    // its contents will eventually be replaced with something else.
    uint32_t get_ticks() {
        return millis();
    }

    void delay(uint16_t num_ticks) {
        uint32_t final_time = get_ticks() + num_ticks;
        while (get_ticks() != final_time);
    }
}
