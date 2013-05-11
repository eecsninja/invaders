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

#ifndef __AVR__

#include <SDL/SDL.h>

#else

///////////// Begin embedded system definitions ////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include "spipad.h"

#define FOSC 8000000
#define BAUD   57600
#define MYUBRR    16

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
    UCSR0A = (1<<U2X0);
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
    DDRE = (1<<PORTE1);

    stdout = &mystdout;  // Required for printf over UART.
    stderr = &mystdout;  // Required for fprintf(stderr) over UART.
    stdin = &mystdin;    // Required for scanf over UART.
}

// This gets incremented every millisecond by the timer ISR.
static volatile uint32_t ms_counter;

// AVR timer interrupt handler.
ISR(TIMER1_COMPA_vect)
{
    ++ms_counter;
}

// Initializes AVR timer.
static void init_timer() {
   TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
   TIMSK |= (1 << OCIE1A); // Enable CTC interrupt
   sei();                  // Enable global interrupts
   OCR1A   = 125;          // Set CTC compare value to trigger at 1 kHz given
                           // an 8-MHz clock with prescaler of 64.
   TCCR1B |= ((1 << CS11) | (1 << CS10)); // Start timer at Fcpu/64

   ms_counter = 0;        // Reset millisecond counter
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
    init_timer();
    init_external_memory();
}

#endif  // !defined(__AVR__)

///////////// End embedded system definitions ////////////////

namespace System {

    bool init() {
#ifndef __AVR__
        atexit(SDL_Quit);
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
            fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
            return false;
        }
#else
        system_init();
        spipad_init();
#endif
        return true;
    }

    KeyState get_key_state() {
        KeyState key_state;
        memset(&key_state, 0, sizeof(key_state));

#ifndef __AVR__
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
#else
        SpiPadButtons buttons = spipad_read();
        if (buttons.A)
            key_state.quit = 1;
        if (buttons.START)
            key_state.pause = 1;
        if (buttons.LEFT)
            key_state.left = 1;
        if (buttons.RIGHT)
            key_state.right = 1;
        if (buttons.B)
            key_state.fire = 1;
#endif  // !defined(__AVR__)
        return key_state;
    }

    // This is just a wrapper around SDL_GetTicks.  As part of the embedded port,
    // its contents will eventually be replaced with something else.
    uint32_t get_ticks() {
#ifndef __AVR__
        return SDL_GetTicks();
#else
        return ms_counter;
#endif
    }

}
