/*
 game.cpp
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

#include "game.h"

#include <SDL/SDL.h>

#include "alien.h"
#include "bonus_ship.h"
#include "explosion.h"
#include "player.h"
#include "rand_num_gen.h"
#include "screen.h"
#include "shield_piece.h"
#include "shot.h"
#include "system.h"

#define player_center               ((screen_w - 43) / 2)
#define player_top                 (screen_h - (25 + 35))
#define player_speed                                  300
#define bonus_speed                                   200
#define shot_speed                                   -300
#define alien_shot_speed                              200
#define base_launch_delay                            6000
#define random_list_len                                30
#define free_guy_val                                50000
#define num_player_shots                                9
#define num_explosions                                  5
#define player_init_x_shot_pos            ((43 - 5)  / 2)
#define player_init_y_shot_pos                         16
#define alien_init_x_shot_pos             ((45 - 5)  / 2)
#define alien_init_y_shot_pos                          25
#define MAX_NUM_ALIEN_SHOTS                            32
extern const char* datadir;

//#define FRAME_COUNTER

using GameEntities::GameEntity;
using GameEntities::Player;
using GameEntities::Alien;
using GameEntities::BonusShip;
using GameEntities::Explosion;
using GameEntities::ShieldPiece;
using GameEntities::Shot;

#define ALIEN_ARRAY_WIDTH    12
#define ALIEN_ARRAY_HEIGHT    5
#define NUM_ALIENS      (ALIEN_ARRAY_WIDTH * ALIEN_ARRAY_HEIGHT)
#define ALIEN_BASE_X        100
#define ALIEN_BASE_Y         90
#define ALIEN_STEP_X         50
#define ALIEN_STEP_Y         35

#define ALIEN_SPEED_BOOST            (FIXED_POINT_FACTOR * 1.027)
#define ALIEN_SPEED_BOOST_EXTRA      (FIXED_POINT_FACTOR * 1.15)

// For generating shield pieces.
#define NUM_SHIELD_GROUPS            3
#define SHIELD_GROUP_WIDTH           6
#define SHIELD_GROUP_HEIGHT          4
#define SHIELD_X_OFFSET            110
#define SHIELD_Y_OFFSET            415
#define SHIELD_GROUP_X_SPACING     230
#define SHIELD_PIECE_SIZE           20  // shield height and width
#define NUM_SHIELDS   (NUM_SHIELD_GROUPS * SHIELD_GROUP_WIDTH * SHIELD_GROUP_HEIGHT)

#ifdef EVENT_COUNTER
#define EVENT_COUNTER_LOOP_LIMIT        1000
EventCounter event_counter;
#endif

// a list of image files are loaded into a map and keyed by filename
static const char* image_list[] = { "ship.png", "shot.png",
    "alien-1-1.png",  "alien-1-2.png",  "alien-1-3.png",  "alien-1-4.png",
    "alien-2-1.png",  "alien-2-2.png",  "alien-2-3.png",  "alien-2-4.png",
    "alien-3-1.png",  "alien-3-2.png",  "alien-3-3.png",  "alien-3-4.png",
    "bonus-1-1.png",  "bonus-1-2.png",  "wave_background.png", "background.png",
    "explosion.png", "shield_piece.png", "bonus-2-1.png", "bonus-2-2.png",
    "ui_header.png", "ui_points.png", NULL
};

namespace Game {

    void Game::game_control()
    {
        // Define arrays locally, so they can be freed when the game loop exits.
        Alien alien_array[NUM_ALIENS];
        ShieldPiece shield_array[NUM_SHIELDS];

        Shot player_shot_array[num_player_shots];
        Shot alien_shot_array[MAX_NUM_ALIEN_SHOTS];
        Explosion explosion_array[num_explosions];

        int direction_array[random_list_len];
        int bonus_select_array[random_list_len];
        int launch_delay_array[random_list_len];

        GameEntity shield_group_array[NUM_SHIELD_GROUPS];

        aliens = alien_array;
        shields = shield_array;

        player_shots = player_shot_array;
        alien_shots = alien_shot_array;
        explosions = explosion_array;

        direction = direction_array;
        bonus_select = bonus_select_array;
        launch_delay = launch_delay_array;

        shield_groups = shield_group_array;

        // Instantiate these here, instead of allocating from heap.
        Player player_obj;
        BonusShip bonus_obj;
        BonusShip small_bonus_obj;

        player = &player_obj;
        rbonus = bonus = &bonus_obj;
        sbonus = &small_bonus_obj;

        // init for new game
        wave = score = 0;
        next_free_guy = free_guy_val;
        aliens_landed = false;
        alien_speed = 80;
        player_life = 3;

        init_wave();
        game_loop();
    }
    void Game::init_wave()
    {
        // create conditions for next wave
        logic_this_loop = wave_over = false;
        screen_updates = last_shot = 0;
        player_shot_counter = alien_shot_counter = explosion_counter = 0;
        factory();
        // output wave message and status display
        //ui.wave_msg(++wave);
        //sound.play_start_wave();
        SDL_BlitSurface(wave_background, NULL, screen, NULL);
        SDL_UpdateRect(screen,0,0,0,0);
        //status.blit_wave(wave);
        //status.blit_score(score);
        //status.blit_lives(player_life);
        //status.blit_player_ships(player_life, get_image("ship.png"));
    }
    void Game::init_aliens(int rand_max, int speed)
    {
        alien_count = 0;
        // create a block of aliens (5 rows, by 12 columns), 800x600 res
        for (int i = 0; i < ALIEN_ARRAY_HEIGHT; ++i) {
            for (int j = 0; j < ALIEN_ARRAY_WIDTH; ++j) {
                int alien_x = ALIEN_BASE_X+(j*ALIEN_STEP_X);
                int alien_y = ALIEN_BASE_Y+(i*ALIEN_STEP_Y);
                bool active = false;
                int type = 0;
                switch (i) {
                case 4:
                    // initialize the bottom row of aliens to fire
                    active = true;
                    type = GAME_ENTITY_ALIEN;
                    break;
                case 3:
                    active = false;
                    type = GAME_ENTITY_ALIEN;
                    break;
                case 2:
                case 1:
                    active = false;
                    type = GAME_ENTITY_ALIEN2;
                    break;
                case 0:
                default:
                    active = false;
                    type = GAME_ENTITY_ALIEN3;
                    break;
                }
                aliens[alien_count].Alien_init(type, alien_x, alien_y, -speed,
                                               0, active, this, alien_count+1,
                                               RAND(rand_max));
                ++alien_count;
            }
        }
        // create alien shots
        for (int i= 0; i < num_alien_shots; ++i) {
            alien_shots[i].Shot_init(0, 0, 0, alien_shot_speed, false, this);
        }
    }
    void Game::factory()
    {
        // create the player ship and place it in the center of the screen
        player->Player_init(player_center, player_top, 0, 0, true, this);

        // create bonus ship and small bonus ship
        bonus->BonusShip_init(false, 0, 0, 0, 0, false, this);
        sbonus->BonusShip_init(true, 0, 0, 0, 0, false, this);

        // create the shields
        int num_shields = 0;

        for (int j = 0; j < NUM_SHIELD_GROUPS; ++j) {
            for (int k = 0; k < SHIELD_GROUP_HEIGHT; ++k) {
                for (int i = 0; i < SHIELD_GROUP_WIDTH - ((k == 0) ? 2 : 0); ++i) {
                    int x = (j * SHIELD_GROUP_X_SPACING) +
                            SHIELD_X_OFFSET + ((k == 0) ? 20 : 0) +
                            (i * SHIELD_PIECE_SIZE);
                    int y = SHIELD_Y_OFFSET + SHIELD_PIECE_SIZE * k;
                    shields[num_shields++].ShieldPiece_init(x, y, 0, 0, true, this);
                }
            }

            shield_groups[j].init(GAME_ENTITY_SHIELD_GROUP,
                                  j * SHIELD_GROUP_X_SPACING + SHIELD_X_OFFSET,
                                  SHIELD_Y_OFFSET, 0, 0, true, this);
        }
        GameEntities::GameEntityTypeProperties prop;
        prop.coll_w = SHIELD_GROUP_WIDTH * SHIELD_PIECE_SIZE;
        prop.coll_h = SHIELD_GROUP_HEIGHT * SHIELD_PIECE_SIZE;
        prop.coll_x_offset = 0;
        prop.coll_y_offset = 0;
        GameEntity::set_type_property(GAME_ENTITY_SHIELD_GROUP, prop);

        // create explosions and player shots
        for (int i= 0; i < num_explosions; ++i) {
            explosions[i].Explosion_init(0, 0, 0, 0, false, this);
        }
        for (int i= 0; i < num_player_shots; ++i) {
            player_shots[i].Shot_init(0, 0, 0, shot_speed, false, this);
        }

        player_shot_delay = 225;
        bonus_launch_delay = base_launch_delay;
        // increase difficulty and chance for bonus points as waves progress
        switch (wave) {
        case 0:
            for (int i = 0; i < random_list_len; ++i) {
                direction[i] = (gen1_2());
                bonus_select[i] = (gen1_5());
                launch_delay[i] = (gen1_4());
            }
            alien_shot_delay = 400;
            num_alien_shots = 16;
            alien_odd_range = 10;
            break;
        case 1:
            for (int i = 0; i < random_list_len; ++i) {
                direction[i] = (gen1_2());
                bonus_select[i] = (gen1_5());
                launch_delay[i] = (gen1_4());
            }
            alien_shot_delay = 375;
            alien_speed += 5;
            num_alien_shots = 17;
            alien_odd_range = 9;
            break;
        case 2:
            for (int i = 0; i < random_list_len; ++i) {
                direction[i] = (gen1_2());
                bonus_select[i] = (gen1_4());
                launch_delay[i] = (gen1_3());
            }
            alien_shot_delay = 350;
            alien_speed += 5;
            num_alien_shots = 20;
            alien_odd_range = 8;
            break;
        case 3:
            for (int i = 0; i < random_list_len; ++i) {
                direction[i] = (gen1_2());
                bonus_select[i] = (gen1_4());
                launch_delay[i] = (gen1_3());
            }
            alien_shot_delay = 325;
            alien_speed += 5;
            num_alien_shots = 20;
            alien_odd_range = 8;
            break;
        case 4:
            for (int i = 0; i < random_list_len; ++i) {
                direction[i] = (gen1_2());
                bonus_select[i] = (gen1_3());
                launch_delay[i] = (gen1_3());
            }
            alien_shot_delay = 300;
            alien_speed += 5;
            num_alien_shots = 21;
            alien_odd_range = 7;
            break;
        case 5:
            for (int i = 0; i < random_list_len; ++i) {
                direction[i] = (gen1_2());
                bonus_select[i] = (gen1_2());
                launch_delay[i] = (gen1_3());
            }
            alien_shot_delay = 300;
            alien_speed += 5;
            num_alien_shots = 22;
            alien_odd_range = 6;
            break;
        default:
            for (int i = 0; i < random_list_len; ++i) {
                direction[i] = (gen1_2());
                bonus_select[i] = (gen1_2());
                launch_delay[i] = (gen1_2());
            }
            if (alien_shot_delay > 200) {
                alien_shot_delay -= 20;
                num_alien_shots += 3;
                if (num_alien_shots > MAX_NUM_ALIEN_SHOTS)
                    num_alien_shots = MAX_NUM_ALIEN_SHOTS;
            }
            alien_speed += 3;
            alien_odd_range = 6;
            break;
        }
        init_aliens(alien_odd_range, alien_speed);
#ifdef FRAME_COUNTER
        printf("wave %d\n", wave + 1);
#endif
    }
    void Game::game_loop()
    {
        clip.x = 0; clip.y = 50; clip.w = screen_w; clip.h = 515;
        SDL_SetClipRect(screen, &clip);
        SDL_Event event;
        Uint8* keys;
        int reloading = 0;
        last_bonus_launch = last_alien_shot = last_loop_time = system_get_ticks();

#ifdef EVENT_COUNTER
        event_counter.reset();
#endif
        while (1) {
#ifdef EVENT_COUNTER
            event_counter.new_loop();
#endif
            // used to calculate how far the entities should move this loop
            // delta is the number of milliseconds the last loop iteration took
            // movement is a function of delta
            delta = system_get_ticks() - last_loop_time;
            last_loop_time = system_get_ticks();

            // background track
            //sound.play_bg(alien_count);

#ifdef FRAME_COUNTER
            // frame counter
            static Uint32 last_fps_time;
            static int fps;
            last_fps_time += delta;
            ++fps;
            // update fps counter
            if (last_fps_time >= 1000) {
                printf("FPS: %d\n", fps);
                last_fps_time = 0;
                fps = 0;
            }
#endif

            // poll input queue
            keys = SDL_GetKeyState(NULL);
            while (SDL_PollEvent(&event)) {
                if (keys[SDLK_ESCAPE]) {
                    //sound.halt_all_sounds();
                    player_dead = true;
                    player_life = 0;
                    return;
                }
                if (keys[SDLK_p]) {
                    pause();
                }
            }

            // erase everything
            if (player->is_active())
                player->erase();
            if (bonus->is_active())
                bonus->erase();
            for (int i = 0; i < NUM_ALIENS; ++i)
              aliens[i].erase();
            for (int i = 0; i < num_player_shots; ++i) {
                if (player_shots[i].is_active()) {
                    player_shots[i].erase();
                }
            }
            for (int i = 0; i < num_alien_shots; ++i) {
                if (alien_shots[i].is_active()) {
                    alien_shots[i].erase();
                }
            }
            for (int i = 0; i < num_explosions; ++i) {
                if (explosions[i].is_active()) {
                    explosions[i].erase();
                }
            }
            for (int i = 0; i < NUM_SHIELDS; ++i) {
                if(shields[i].is_alive())
                    shields[i].erase();
            }

            // player attempt to fire
            if (!reloading) {
                if (keys[SDLK_SPACE] == SDL_PRESSED) {
                    fire_shot();
                }
            }
            reloading = (keys[SDLK_SPACE] == SDL_PRESSED);


            // set player direction based on key input
            player->set_x_velocity(0);
            if (keys[SDLK_LEFT] && !keys[SDLK_RIGHT]) {
                player->set_x_velocity(-player_speed);
            }
            if (keys[SDLK_RIGHT] && !keys[SDLK_LEFT]) {
                player->set_x_velocity(player_speed);
            }

            // alien behavior
            alien_fire();
            launch_bonus_ship();

            // move everything
            if (player->is_active())
                player->movement(delta);
            if (bonus->is_active()) {
                //sound.play_bonus();
                bonus->movement(delta);
            } else {
                //sound.halt_bonus();
            }
            for (int i = 0; i < NUM_ALIENS; ++i) {
                if (aliens[i].is_alive())
                    aliens[i].movement(delta);
            }
            for (int i = 0; i < num_player_shots; ++i) {
                if (player_shots[i].is_active()) {
                    player_shots[i].movement(delta);
                }
            }
            for (int i = 0; i < num_alien_shots; ++i) {
                if (alien_shots[i].is_active()) {
                    alien_shots[i].movement(delta);
                }
            }
            // explosion duration
            for (int i = 0; i < num_explosions; ++i) {
                if (explosions[i].is_active()) {
                    explosions[i].duration(delta);
                }
            }

            // collision handling
            num_entities_removed = 0;
            // alien shots with player and shields
            for (int i = 0; i < num_alien_shots; ++i) {
                Shot* shot = &alien_shots[i];
                if (!shot->is_active())
                    continue;

                if (player->collides_with(shot)) {
                    player->player_shot_collision(shot);
                    if (!shot->is_active())
                        continue;
                }
                for (int i = 0; i < NUM_SHIELDS && collides_with_shield_group(shot); ++i) {
                    ShieldPiece* shield = &shields[i];
                    if (shield->is_alive() && shield->collides_with(shot)) {
                        shot->shot_shield_collision(shield);
                        if (!shot->is_active())
                            continue;
                    }
                }
            }
            // shots with shots
            for (int j = 0; j < num_player_shots; ++j) {
                Shot* shot = &player_shots[j];
                if (!shot->is_active())
                    continue;

                  for (int i = 0; i < num_alien_shots; ++i) {
                      Shot* alien_shot = &alien_shots[i];
                      if (!alien_shot->is_active())
                          continue;
                      if (shot->collides_with(alien_shot)) {
                          //sound.play_shot_collision();
                          shot->shot_shot_collision(alien_shot);
                          if (!shot->is_active())
                              continue;
                      }
                  }
            }
            // player shots with aliens, bonus, and shields
            for (int j = 0; j < num_player_shots; ++j) {
                Shot* shot = &player_shots[j];
                if (!shot->is_active())
                    continue;
                if (!bonus->is_active() && shot->collides_with(bonus)) {
                    bonus->bonus_shot_collision(shot);
                    if (!shot->is_active())
                        continue;
                }
                for (int i = 0; i < NUM_ALIENS; ++i) {
                    if (aliens[i].is_alive() && shot->collides_with(&aliens[i])) {
                        shot->shot_alien_collision(&aliens[i]);
                        if (!shot->is_active())
                            continue;
                    }
                }
                for (int i = 0; i < NUM_SHIELDS && collides_with_shield_group(shot); ++i) {
                    ShieldPiece* shield = &shields[i];
                    if (shield->is_alive() && shield->collides_with(shot)) {
                        shot->shot_shield_collision(shield);
                        if (!shot->is_active())
                            continue;
                    }
                }
            }
            // aliens with shields and player
            for (int j = 0; j < NUM_ALIENS; ++j) {
                Alien* alien = &aliens[j];
                if (!alien->is_alive())
                    continue;
                for (int i = 0; i < NUM_SHIELDS && collides_with_shield_group(alien); ++i) {
                    ShieldPiece* shield = &shields[i];
                    if (shield->is_alive() && shield->collides_with(alien)) {
                        alien->alien_shield_collision(shield);
                        if (!alien->is_alive())
                            continue;
                    }
                }
                if (player->is_active() && player->collides_with(alien)) {
                    player->player_alien_collision(alien);
                    if (!alien->is_alive())
                        continue;
                }
            }

            // the conditions to break out of the game loop
            // the order of these matters
            if (aliens_landed) {
                //sound.halt_bonus();
                //sound.halt_all_bg();
                //sound.wait_for_all_to_finish();
                //sound.play_aliens_landed();
                //ui.screen_msg("Game Over");
                //sound.play_game_over();
                //ui.check_high_scores(score, wave);
                return;
            }
            // conditions for player death by alien shot or collision
            if (player_dead && no_explosions_active()) {
                --player_life;
                //status.blit_lives(player_life);
                //sound.halt_bonus();
                //sound.halt_all_bg();
                if (!player_life) {
                    //sound.halt_all_bg();
                    //sound.play_player_dead();
                    //sound.wait_for_all_to_finish();
                    //ui.screen_msg("Game Over");
                    //sound.play_game_over();
                    //ui.check_high_scores(score, wave);
                    return;
                }
                dead_pause = system_get_ticks();
                //sound.play_player_dead();
                //status.erase_player_ship(player_life, get_image("ship.png"));
                player_rebirth();
                //sound.play_bonus();
                //sound.play_bg(alien_count);
                last_loop_time += system_get_ticks() - dead_pause;
                last_bonus_launch = last_alien_shot = system_get_ticks();
                // erase player and alien shots to give player a chance to continue
                for (int i = 0; i < num_alien_shots; ++i) {
                    alien_shots[i].deactivate();
                }
                for (int i = 0; i < num_player_shots; ++i) {
                    player_shots[i].deactivate();
                }
                player_dead = false;
            }
            // conditions for end of wave
            //if (wave_over) sound.halt_all_bg();
            if (wave_over && !bonus->is_active()) {
                //sound.halt_bonus();
                bonus_launch_delay = 100000;
                player_shot_delay = 100000;
                // wait for explosions and shots to finish
                if (no_explosions_active() && no_player_shots_active() && no_alien_shots_active()) {
                    wave_cleanup();
                    //sound.wait_for_all_to_finish();
                    //sound.play_end_wave();
                    return;
                }
            }

            // run alien logic if neccessary
            if (logic_this_loop) {
                for (int i = 0; i < NUM_ALIENS; ++i) {
                    aliens[i].do_alien_logic();
                }
                logic_this_loop = false;
            }

            // draw everything
            if (player->is_active())
                player->draw();
            if (bonus->is_active())
                bonus->draw();

            for (int i = 0; i < NUM_ALIENS; ++i) {
                if (aliens[i].is_alive())
                  aliens[i].draw();
            }
            for (int i = 0; i < num_player_shots; ++i) {
                if (player_shots[i].is_active()) {
                    player_shots[i].draw();
                }
            }
            for (int i = 0; i < num_alien_shots; ++i) {
                if (alien_shots[i].is_active()) {
                    alien_shots[i].draw();
                }
            }
            for (int i = 0; i < num_explosions; ++i) {
                if (explosions[i].is_active()) {
                    explosions[i].draw();
                }
            }
            for (int i = 0; i < NUM_SHIELDS; ++i) {
                if (shields[i].is_alive())
                    shields[i].draw();
            }
            // update screen
            for (int i = 0; i < screen_updates; ++i) {
                SDL_BlitSurface(blits[i].img, &blits[i].src, screen, &blits[i].dst);
            }
            SDL_UpdateRect(screen, 0, 0, 0, 0);
            screen_updates = 0;

#ifdef EVENT_COUNTER
            if (event_counter.num_loops > 0 &&
                event_counter.num_loops % EVENT_COUNTER_LOOP_LIMIT == 0) {
                event_counter.report();
            }
#endif
        }
    }
    void Game::player_rebirth()
    {
        //erase everything
        SDL_BlitSurface(wave_background, NULL, screen, NULL);
        // re-init player
        player->init_x(player_center);
        player->init_y(player_top);
        player->activate();
        player_shot_delay = 225;
        player->cleanup_draw();
        if (bonus->is_active()) {
            bonus->cleanup_draw();
        }
        for (int i = 0; i < NUM_SHIELDS; ++i) {
            if (shields[i].is_alive())
                shields[i].cleanup_draw();
        }
        for (int i = 0; i < NUM_ALIENS; ++i) {
            aliens[i].cleanup_draw();
        }
        // update screen
        SDL_UpdateRect(screen,clip.x,clip.y,clip.w,clip.h);
        //sound.play_player_rebirth();
    }
    void Game::wave_cleanup()
    {
        //erase last alien shots, explosions, and shieldpieces
        SDL_BlitSurface(wave_background, NULL, screen, NULL);
        // redraw all that should remain
        if (player->is_active())
            player->cleanup_draw();
        for (int i = 0; i < NUM_SHIELDS; ++i) {
            shields[i].cleanup_draw();
        }
        // update screen
        SDL_UpdateRect(screen,clip.x,clip.y,clip.w,clip.h);
    }
    void Game::pause()
    {
        Uint32 begin_pause;
        Uint8* keys;
        SDL_Event event;
        //sound.halt_bonus();
        //sound.halt_bg(alien_count);
        begin_pause = system_get_ticks();
        while (1) {
            keys = SDL_GetKeyState(NULL);
            while (SDL_PollEvent(&event)) {
                if (keys[SDLK_p]) {
                    last_loop_time +=  system_get_ticks() - begin_pause;
                    last_bonus_launch = last_alien_shot = system_get_ticks();
                    //sound.play_bonus();
                    return;
                }
            }
        }
    }
    void Game::fire_shot()
    {
        // check that player has waited long enough to fire
        if (system_get_ticks() - last_shot < player_shot_delay) {
            return;
        }
        // record time and fire
        last_shot = system_get_ticks();
        player_shots[player_shot_counter].init_x(player->get_x()+player_init_x_shot_pos);
        player_shots[player_shot_counter].init_y(player->get_y()-player_init_y_shot_pos);
        player_shots[player_shot_counter].set_hit(false);
        player_shots[player_shot_counter].activate();
        if (++player_shot_counter == num_player_shots) {
            player_shot_counter = 0;
        }
        //sound.play_shot();
    }
    void Game::launch_bonus_ship()
    {
#define top 60
        if (system_get_ticks() - last_bonus_launch < bonus_launch_delay) {
            return;
        }
        last_bonus_launch = system_get_ticks();
        static int rand_list_count = 0;
        if (bonus_select[rand_list_count] == 1) {
            bonus = sbonus;
        } else {
            bonus = rbonus;
        }
        if (direction[rand_list_count] == 1) {
            bonus->init_x(0);
            bonus->init_y(top);
            bonus->set_x_velocity(bonus_speed);
            bonus->activate();
        } else {
            bonus->init_x(screen_w);
            bonus->init_y(top);
            bonus->set_x_velocity(-bonus_speed);
            bonus->activate();
        }
        //sound.play_bonus();
        bonus_launch_delay = base_launch_delay + (1000 * launch_delay[rand_list_count]);
        if (++rand_list_count == random_list_len) {
            rand_list_count = 0;
        }
#undef top
    }
    void Game::alien_fire()
    {
        // check that aliens have waited long enough to fire
        if (system_get_ticks() - last_alien_shot < alien_shot_delay) {
            return;
        }
        // record time and fire
        last_alien_shot = system_get_ticks();
        static int alien_to_fire = 0;
        ++alien_to_fire;
        for (int i = 0; i < NUM_ALIENS; ++i) {
            Alien* alien = &aliens[i];
            if (alien->is_active() && alien->get_fire_chance() == alien_to_fire) {
                alien_shots[alien_shot_counter].init_x(alien->get_x()+alien_init_x_shot_pos);
                alien_shots[alien_shot_counter].init_y(alien->get_y()+alien_init_y_shot_pos);
                alien_shots[alien_shot_counter].set_hit(false);
                alien_shots[alien_shot_counter].activate();
                if (++alien_shot_counter == num_alien_shots) {
                    alien_shot_counter = 0;
                }
            }
        }
        if (alien_to_fire >= alien_odd_range) alien_to_fire = 0;
    }
    void Game::msg_alien_player_collide()
    {
        //sound.play_explode();
        player_dead = true;
        if (!(--alien_count)) {
            wave_over = true;
        }
    }
    void Game::msg_alien_landed()
    {
        player_dead = aliens_landed = true;
        player_life = 0;
    }
    void Game::msg_player_dead()
    {
        player_shot_delay = 100000;
        player_dead = true;
        //sound.play_explode();
    }
    inline void Game::free_guy_check()
    {
        if (score >= next_free_guy) {
            //sound.play_free_guy();
            ++player_life;
            //status.blit_lives(player_life);
            //status.blit_player_ships(player_life, get_image("ship.png"));
            next_free_guy += free_guy_val;
        }
    }
    void Game::msg_bonus_ship_destroyed(int bonus)
    {
        score += bonus;
        if (bonus == 1000) {
            //sound.play_rbonus();
        } else {
            //sound.play_sbonus();
        }
        //status.blit_score(score);
        free_guy_check();
    }
    void Game::msg_alien_killed(int pos, int points)
    {
        //sound.play_alien_dead();
        score += points;
        free_guy_check();
        //status.blit_score(score);
        // when all aliens are destroyed, wave over
        if (--alien_count == 0) {
            wave_over = true;
            return;
        }
        // set the alien above the one just killed to active
        // speed up all the existing aliens, whenever one is destroyed
        for (int i = 0; i < NUM_ALIENS; ++i) {
            Alien* alien = &aliens[i];
            if (alien->get_pos() == pos - 12) {
                alien->activate();
            }
            alien->increase_x_speed(ALIEN_SPEED_BOOST);
            switch (alien_count) {
                case 4:
                    alien->increase_x_speed(ALIEN_SPEED_BOOST_EXTRA);
                    //sound.halt_bg(alien_count);
                    break;
                case 3:
                    //sound.halt_bg(alien_count);
                    break;
                case 2:
                    alien->increase_x_speed(ALIEN_SPEED_BOOST_EXTRA);
                    //sound.halt_bg(alien_count);
                    break;
                case 1:
                    alien->increase_x_speed(ALIEN_SPEED_BOOST_EXTRA);
                    //sound.halt_bg(alien_count);
                    break;
                default:
                    break;
            }
        }
    }
    bool Game::collides_with_shield_group(GameEntity* object) {
        for (int i = 0; i < NUM_SHIELD_GROUPS; ++i) {
            GameEntity* shield_group = &shield_groups[i];
            if (shield_group->collides_with(object))
                return true;
        }
        return false;
    }
    bool Game::no_player_shots_active() {
        for (int i = 0; i < num_player_shots; ++i) {
            if (player_shots[i].is_active())
                return false;
        }
        return true;
    }
    bool Game::no_alien_shots_active() {
        for (int i = 0; i < num_alien_shots; ++i) {
            if (alien_shots[i].is_active())
                return false;
        }
        return true;
    }
    bool Game::no_explosions_active() {
        for (int i = 0; i < num_explosions; ++i) {
            if (explosions[i].is_active())
                return false;
        }
        return true;
    }
    void Game::explode(fixed x, fixed y, uint32_t duration)
    {
        explosions[explosion_counter].init_x(x);
        explosions[explosion_counter].init_y(y);
        explosions[explosion_counter].set_explosion(duration);
        if (++explosion_counter == num_explosions) {
            explosion_counter = 0;
        }
    }
    Game::Game() : ui(&sound, this, 0),
                   player_life(0),
                   player(NULL),
                   bonus(NULL),
                   sbonus(NULL),
                   rbonus(NULL)
    {
        set_video_mode(false);
        images.load_images(image_list);
        wave_background = get_image("wave_background.png");
        background = get_image("background.png");
        ui_header = get_image("ui_header.png");
        ui_points = get_image("ui_points.png");
    }
    Game::~Game()
    {
        free_entities();
    }
    void Game::free_entities()
    {
      if (player) {
        delete player;
        player = NULL;
      }
      if (rbonus) {
        delete rbonus;
        rbonus = NULL;
      }
      if (sbonus) {
        delete sbonus;
        sbonus = NULL;
      }
      bonus = NULL;
    }

    SDL_Surface* Game::get_image(const char* filename) {
        return images.get_image(filename);
    }
}
