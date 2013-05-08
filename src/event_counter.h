/*
 event_counter.h
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

#ifndef EVENT_COUNTER_H
#define EVENT_COUNTER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "system.h"

#define EVENT_COUNTER_LOOP_LIMIT        100

// To improve performance, count the number of various calls per game cycle.
// This should reveal the bottlenecks where optimization could help.
class EventCounter {
  private:
    // Total number of various types of game loop operations.
    uint32_t num_collision_checks;
    uint32_t num_duration_calls;
    uint32_t num_movement_calls;

    // Total number of game loops elapsed.
    uint32_t num_loops;

    uint32_t game_loop_start_time;     // Time entered into game loop.
    uint32_t latest_time;              // Last time a game loop completed.

    // Total time executing game logic.  Does not include hardware waits.
    uint32_t game_logic_time;
    // Used by start_game_logic_section() and end_game_logic_section().
    uint32_t game_logic_section_start_time;

  public:
    EventCounter() {
        reset();
    }

    ~EventCounter() {
        report();
    }

    void reset() {
        memset(this, 0, sizeof(*this));
        latest_time = game_loop_start_time = System::get_ticks();
    }

    void do_collision_check() {
        ++num_collision_checks;
    }

    void do_duration_call() {
        ++num_duration_calls;
    }

    void do_movement_call() {
        ++num_movement_calls;
    }

    void start_game_logic_section() {
        game_logic_section_start_time = System::get_ticks();
    }

    void end_game_logic_section() {
        game_logic_time += System::get_ticks() - game_logic_section_start_time;
    }

    void new_loop() {
        ++num_loops;
        latest_time = System::get_ticks();

        // Report the latest stats if necessary.
        if (num_loops > 0 && num_loops % EVENT_COUNTER_LOOP_LIMIT == 0)
            report();
    }

    void report() {
        if (num_loops == 0)
            return;
        printf("Per-loop stats, averaged over %d loops:\n", num_loops);
        printf("- Collision checks: %d\n", num_collision_checks / num_loops);
        printf("- Duration calls: %d\n", num_duration_calls / num_loops);
        printf("- Movement calls: %d\n", num_movement_calls / num_loops);
        printf("- Loop time in ticks: %d\n",
               (latest_time - game_loop_start_time) / num_loops);
        printf("- Game logic time in ticks: %d\n", game_logic_time / num_loops);
    }
};

#endif  // EVENT_COUNTER_H
