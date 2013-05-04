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

#ifdef __i386__

#include <SDL/SDL.h>

#else

///////////// Begin embedded system definitions ////////////////

#include <avr/io.h>

#define FOSC 8000000
#define BAUD 9600
#define MYUBRR 12

static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static void init_fdev(FILE* stream,
                      int (*put_func)(char, FILE*),
                      int (*get_func)(FILE*),
                      int flags) {
    stream->flags = flags;
    stream->put = put_func;
    stream->get = get_func;
    stream->udata = NULL;
}

static FILE mystdout;
static FILE mystdin;

static int uart_putchar(char c, FILE *stream) {
    if (c == '\n')
        uart_putchar('\r', stream);
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;

    return 0;
}

static int uart_getchar(FILE *stream) {
    while( !(UCSR0A & (1<<RXC0)) );
    return(UDR0);
}

// Initializes AVR UART.
static void init_uart() {
    UBRR0H = MYUBRR >> 8;
    UBRR0L = MYUBRR;
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
    DDRE = (1<<PORTE1);

    stdout = &mystdout;  // Required for printf over UART.
    stdin = &mystdin;    // Required for scanf over UART.
}

// Initializes AVR external memory.
static void init_external_memory() {
    MCUCR = (1<<SRE);
    XMCRB = (1<<XMBK) | (1<<XMM0);
    DDRC = 0xff;
    PORTC = 0;
}

static void system_init() {
    init_fdev(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
    init_fdev(&mystdin, NULL, uart_getchar, _FDEV_SETUP_READ);
    init_uart();
    init_external_memory();
}

#endif  // defined(__i386__)

///////////// End embedded system definitions ////////////////

namespace System {

    bool init() {
#ifdef __i386__
        atexit(SDL_Quit);
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
            fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
            return false;
        }
#else
        system_init();
#endif
        return true;
    }

    KeyState get_key_state() {
        KeyState key_state;
        memset(&key_state, 0, sizeof(key_state));

#ifdef __i386__
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
#endif  // defined(__i386__)
        return key_state;
    }

    // This is just a wrapper around SDL_GetTicks.  As part of the embedded port,
    // its contents will eventually be replaced with something else.
    uint32_t get_ticks() {
        uint32_t ticks = 0;
#ifdef __i386__
        ticks = SDL_GetTicks();
#endif
        return ticks;
    }

}
