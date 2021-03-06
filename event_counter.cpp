/*
 event_counter.cpp
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

#include "event_counter.h"

#include "printf.h"

void EventCounter::new_loop()
{
    ++num_loops;
    latest_time = System::get_ticks();

    // Report the latest stats if necessary.
    if (num_loops > 0 && num_loops == EVENT_COUNTER_LOOP_LIMIT) {
        report();
        reset();
    }
}

void EventCounter::report()
{
    if (num_loops == 0)
        return;
    printf_P("Average stats over last %d loops:\n", num_loops);
    printf_P("- Collision checks: %d\n", num_collision_checks / num_loops);
    printf_P("- Movement calls: %d\n", num_movement_calls / num_loops);
    printf_P("- Loop time in ticks: %d\n",
             (latest_time - game_loop_start_time) / num_loops);
    uint32_t total_game_logic_time = 0;
    printf_P("- Per-section game logic times: ");
    for (int i = 0; i < MAX_GAME_LOGIC_SECTIONS; ++i) {
        total_game_logic_time += game_logic_times[i];
        if (game_logic_times[i] >= num_loops)
            printf_P("%lu ", game_logic_times[i] / num_loops);
        else    // If the rounded-down value is zero, don't bother printing it.
            printf_P("_ ");   // Just print a placeholder.
    }
    printf_P("\n");
    printf_P("- Total game logic time in ticks: %d\n",
             total_game_logic_time / num_loops);
}
