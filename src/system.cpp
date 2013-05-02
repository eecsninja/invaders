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

#include <string.h>

//#include <SDL/SDL.h>

namespace System {

    KeyState get_key_state() {
        KeyState key_state;
        memset(&key_state, 0, sizeof(key_state));

/*
        // Get the current keys from SDL.  Call SDL_PollEvent() to update the
        // key array with the latest values.
        Uint8* keys = SDL_GetKeyState(NULL);
        SDL_Event event;
        while(SDL_PollEvent(&event));

        if (keys[SDLK_ESCAPE])
            key_state.quit = 1;
        if (keys[SDLK_p])
            key_state.pause = 1;
        if (keys[SDLK_LEFT])
            key_state.left = 1;
        if (keys[SDLK_RIGHT])
            key_state.right = 1;
        if (keys[SDLK_SPACE])
            key_state.fire = 1;
*/
        return key_state;
    }

    // This is just a wrapper around SDL_GetTicks.  As part of the embedded port,
    // its contents will eventually be replaced with something else.
    uint32_t get_ticks() {
        //return SDL_GetTicks();
        return 0;
    }

}
