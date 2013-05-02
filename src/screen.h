/*
 screen.h
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

#ifndef SCREEN_H
#define SCREEN_H

#include <SDL/SDL.h>

#define screen_w      800
#define screen_h      600
#define side_padding    5
extern SDL_Surface* screen;
extern SDL_Surface* background;
extern SDL_Surface* wave_background;
extern SDL_Surface* ui_header;
extern SDL_Surface* ui_points;
extern SDL_Rect clip;
#define max_updates   360

namespace Graphics {

    class Images;

    class Video {
    private:
        // A list of scheduled blits.
        struct blit {
            int image_index;
            int x;
            int y;
        } blits[max_updates];
        int num_blits;
        Images* image_lib;

        SDL_Surface *screen;       // Video screen
        // Background/UI images
        SDL_Surface *background, *wave_background, *ui_header, *ui_points;

        SDL_Rect clip;   // Clipping rectangle.

    public:
        Video() : num_blits(0),
                  image_lib(NULL) {}

        // Initialize video.  Returns true on success.
        bool init();

        // Provide a pointer to an image library.
        void set_image_lib(Images* images);

        // Set the operating video mode.
        void set_video_mode(bool fullscreen);

        // Add a blit task to the blit queue.
        void schedule_blit(int image_index, int x, int y);

        // Perform all queued blits and reset the blit counter.
        void flush_blits();

        // Redraw the entire screen, including all the scheduled blits.
        void update_screen();
    };

}  // namespace Graphics

#endif  //SCREEN_H
