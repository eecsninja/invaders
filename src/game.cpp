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
#include "screen.h"
#include "player.h"
#include "alien.h"
#include "shot.h"
#include "bonus_ship.h"
#include "explosion.h"
#include "shield_piece.h"
#include "rand_num_gen.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL.h>
#include <iostream>
#include <ctime>
#include <boost/assign/list_of.hpp>
#include <sstream>

static const int player_center = (screen_w - 43) / 2;
static const int player_top = screen_h - (25 + 35);
static const int player_speed = 300;
static const int bonus_speed = 200;
static const int shot_speed = -300;
static const int alien_shot_speed = 200;
static const int base_launch_delay = 6000;
static const int random_list_len = 30;
static const Uint32 free_guy_val = 50000;
static const int num_player_shots = 9;
static const int num_explosions = 5;
static const int player_init_x_shot_pos = (43 - 5)  / 2;
static const int player_init_y_shot_pos = 16;
static const int alien_init_x_shot_pos = (45 - 5)  / 2;
static const int alien_init_y_shot_pos = 25;
extern std::string datadir;

//#define FRAME_COUNTER

namespace Game {

    void Game::game_control()
    {
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
        // clear all lists and vectors and free all memory
        direction.clear();
        bonus_select.clear();
        launch_delay.clear();
        dead_entities.clear();
        aliens.clear();
        player_shots.clear();
        alien_shots.clear();
        explosions.clear();
        shields.clear();
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
    void Game::init_aliens(generator_fun& gen, int speed)
    {
        alien_count = 0;
        // create a block of aliens (5 rows, by 12 columns), 800x600 res
        typedef boost::shared_ptr<GameEntities::GameEntity> s_ptr;
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 12; ++j) {
                switch (i) {
                    case 4:
                    {
                        // initialize the bottom row of aliens to fire
                        s_ptr alien(new GameEntities::Alien(100+(j*50), 90+(i*35), -speed, 0, true, this, alien_count+1, gen()));
                        aliens.push_back(alien);
                        break;
                    }
                    case 3:
                    {
                        s_ptr alien(new GameEntities::Alien(100+(j*50), 90+(i*35), -speed, 0, false, this, alien_count+1, gen()));
                        aliens.push_back(alien);
                        break;
                    }
                    case 2:
                    case 1:
                    {
                        s_ptr alien(new GameEntities::Alien2(100+(j*50), 90+(i*35), -speed, 0, false, this, alien_count+1, gen()));
                        aliens.push_back(alien);
                        break;
                    }
                    case 0:
                    {
                        s_ptr alien(new GameEntities::Alien3(100+(j*50), 90+(i*35), -speed, 0, false, this, alien_count+1, gen()));
                        aliens.push_back(alien);
                        break;
                    }
                    default:
                        break;
                }
                ++alien_count;
            }
        }
        // create alien shots
        for (int i= 0; i < num_alien_shots; ++i) {
            s_ptr shot(new GameEntities::Shot(0, 0, 0, alien_shot_speed, false, this));
            alien_shots.push_back(shot);
        }
    }
    void Game::factory()
    {
        // create the player ship and place it in the center of the screen
        typedef boost::shared_ptr<GameEntities::GameEntity> s_ptr;
        s_ptr sp_player(new GameEntities::Player(player_center, player_top, 0, 0, true, this));
        player = sp_player;
        // create bonus ship and small bonus ship
        s_ptr sp_rbonus(new GameEntities::BonusShip(0, 0, 0, 0, false, this));
        rbonus = bonus = sp_rbonus;
        s_ptr sp_sbonus(new GameEntities::SmallBonusShip(0, 0, 0, 0, false, this));
        sbonus = sp_sbonus;
        // create the shields
        int dim = 20; // shield height and width
        int space = 230;
        for (int j = 0; j < 3; ++j) {
            for (int i = 0; i < 6; ++i) {
                s_ptr shield_piece(new GameEntities::ShieldPiece((j*space)+110+(i*dim), 475, 0, 0, true, this));
                shields.push_back(shield_piece);
            }
            for (int i = 0; i < 6; ++i) {
                s_ptr shield_piece(new GameEntities::ShieldPiece((j*space)+110+(i*dim), 475-dim, 0, 0, true, this));
                shields.push_back(shield_piece);
            }
            for (int i = 0; i < 6; ++i) {
                s_ptr shield_piece(new GameEntities::ShieldPiece((j*space)+110+(i*dim), 475-(dim*2), 0, 0, true, this));
                shields.push_back(shield_piece);
            }
            for (int i = 0; i < 4; ++i) {
                s_ptr shield_piece(new GameEntities::ShieldPiece((j*space)+130+(i*dim), 475-(dim*3), 0, 0, true, this));
                shields.push_back(shield_piece);
            }
        }
        // create explosions and player shots
        for (int i= 0; i < num_explosions; ++i) {
            s_ptr explosion(new GameEntities::Explosion(0, 0, 0, 0, false, this));
            explosions.push_back(explosion);
        }
        for (int i= 0; i < num_player_shots; ++i) {
            s_ptr shot(new GameEntities::Shot(0, 0, 0, shot_speed, false, this));
            player_shots.push_back(shot);
        }

        player_shot_delay = 225;
        bonus_launch_delay = base_launch_delay;
        // increase difficulty and chance for bonus points as waves progress
        switch (wave) {
        case 0:
            for (int i = 0; i < random_list_len; ++i) {
                direction.push_back(gen1_2());
                bonus_select.push_back(gen1_5());
                launch_delay.push_back(gen1_4());
            }
            alien_shot_delay = 400;
            num_alien_shots = 16;
            alien_odd_range = 10;
            init_aliens(gen1_10, alien_speed);
            break;
        case 1:
            for (int i = 0; i < random_list_len; ++i) {
                direction.push_back(gen1_2());
                bonus_select.push_back(gen1_5());
                launch_delay.push_back(gen1_4());
            }
            alien_shot_delay = 375;
            alien_speed += 5;
            num_alien_shots = 17;
            alien_odd_range = 9;
            init_aliens(gen1_9, alien_speed);
            break;
        case 2:
            for (int i = 0; i < random_list_len; ++i) {
                direction.push_back(gen1_2());
                bonus_select.push_back(gen1_4());
                launch_delay.push_back(gen1_3());
            }
            alien_shot_delay = 350;
            alien_speed += 5;
            num_alien_shots = 20;
            alien_odd_range = 8;
            init_aliens(gen1_8, alien_speed);
            break;
        case 3:
            for (int i = 0; i < random_list_len; ++i) {
                direction.push_back(gen1_2());
                bonus_select.push_back(gen1_4());
                launch_delay.push_back(gen1_3());
            }
            alien_shot_delay = 325;
            alien_speed += 5;
            num_alien_shots = 20;
            alien_odd_range = 8;
            init_aliens(gen1_8, alien_speed);
            break;
        case 4:
            for (int i = 0; i < random_list_len; ++i) {
                direction.push_back(gen1_2());
                bonus_select.push_back(gen1_3());
                launch_delay.push_back(gen1_3());
            }
            alien_shot_delay = 300;
            alien_speed += 5;
            num_alien_shots = 21;
            alien_odd_range = 7;
            init_aliens(gen1_7, alien_speed);
            break;
        case 5:
            for (int i = 0; i < random_list_len; ++i) {
                direction.push_back(gen1_2());
                bonus_select.push_back(gen1_2());
                launch_delay.push_back(gen1_3());
            }
            alien_shot_delay = 300;
            alien_speed += 5;
            num_alien_shots = 22;
            alien_odd_range = 6;
            init_aliens(gen1_6, alien_speed);
            break;
        default:
            for (int i = 0; i < random_list_len; ++i) {
                direction.push_back(gen1_2());
                bonus_select.push_back(gen1_2());
                launch_delay.push_back(gen1_2());
            }
            if (alien_shot_delay > 200) {
                alien_shot_delay -= 20;
                num_alien_shots += 3;
            }
            alien_speed += 3;
            alien_odd_range = 6;
            init_aliens(gen1_6, alien_speed);
            break;
        }
#ifdef FRAME_COUNTER
        std::cout << "wave " << wave + 1 << '\n';
#endif
    }
    void Game::game_loop()
    {
        clip.x = 0; clip.y = 50; clip.w = screen_w; clip.h = 515;
        SDL_SetClipRect(screen, &clip);
        SDL_Event event;
        Uint8* keys;
        int reloading = 0;
        last_bonus_launch = last_alien_shot = last_loop_time = SDL_GetTicks();
        while (1) {
            // used to calculate how far the entities should move this loop
            // delta is the number of milliseconds the last loop iteration took
            // movement is a function of delta
            delta = SDL_GetTicks() - last_loop_time;
            last_loop_time = SDL_GetTicks();

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
                std::cout << fps << '\n';
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
                if (event.type == SDL_QUIT) {
                    sdl_quit_event = true;
                    return;
                }
            }

            // erase everything
            if (player->is_active())
                player->erase();
            if (bonus->is_active())
                bonus->erase();
            typedef std::list<boost::shared_ptr<GameEntities::GameEntity> >::iterator Iter;
            for (Iter it = aliens.begin(); it != aliens.end(); ++it) {
                (*it)->erase();
            }
            typedef std::vector<boost::shared_ptr<GameEntities::GameEntity> >::iterator vIter;
            for (vIter it = player_shots.begin(); it != player_shots.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->erase();
                }
            }
            for (vIter it = alien_shots.begin(); it != alien_shots.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->erase();
                }
            }
            for (vIter it = explosions.begin(); it != explosions.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->erase();
                }
            }
            for (Iter it = shields.begin(); it != shields.end(); ++it) {
                (*it)->erase();
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
            for (Iter it = aliens.begin(); it != aliens.end(); ++it) {
                (*it)->movement(delta);
            }
            for (vIter it = player_shots.begin(); it != player_shots.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->movement(delta);
                }
            }
            for (vIter it = alien_shots.begin(); it != alien_shots.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->movement(delta);
                }
            }
            // explosion duration
            for (vIter it = explosions.begin(); it != explosions.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->duration(delta);
                }
            }

            // collision handling
            num_entities_removed = 0;
            // alien shots with player and shields
            for (vIter shot = alien_shots.begin(); shot != alien_shots.end(); ++shot) {
                if ((*shot)->is_active()) {
                    if (player->collides_with(*shot)) {
                        player->player_shot_collision(*shot);
                    }
                    for (Iter shield = shields.begin(); shield != shields.end(); ++shield) {
                        if ((*shield)->collides_with(*shot)) {
                            (*shot)->shot_shield_collision(*shield);
                        }
                    }
                }
            }
            // shots with shots
            for (vIter ps = player_shots.begin(); ps != player_shots.end(); ++ps) {
                if ((*ps)->is_active()) {
                    for (vIter as = alien_shots.begin(); as != alien_shots.end(); ++as) {
                        if ((*as)->is_active()) {
                            if ((*ps)->collides_with(*as)) {
                                //sound.play_shot_collision();
                                (*ps)->shot_shot_collision(*as);
                            }
                        }
                    }
                }
            }
            // player shots with aliens, bonus, and shields
            for (vIter shot = player_shots.begin(); shot != player_shots.end(); ++shot) {
                if ((*shot)->is_active()) {
                    if (bonus->is_active()) {
                        if ((*shot)->collides_with(bonus)) {
                            bonus->bonus_shot_collision(*shot);
                        }
                    }
                    for (Iter alien = aliens.begin(); alien != aliens.end(); ++alien) {
                        if ((*shot)->collides_with(*alien)) {
                            (*shot)->shot_alien_collision(*alien);
                        }
                    }
                    for (Iter shield = shields.begin(); shield != shields.end(); ++shield) {
                        if ((*shield)->collides_with(*shot)) {
                            (*shot)->shot_shield_collision(*shield);
                        }
                    }
                }
            }
            // aliens with shields and player
            for (Iter alien = aliens.begin(); alien != aliens.end(); ++alien) {
                for (Iter shield = shields.begin(); shield != shields.end(); ++shield) {
                    if ((*shield)->collides_with(*alien)) {
                        (*alien)->alien_shield_collision(*shield);
                    }
                }
                if (player->is_active() && player->collides_with(*alien)) {
                    player->player_alien_collision(*alien);
                }
            }

            // remove dead entities from aliens and shields
            {
                int i = 0;
                for (Iter it = dead_entities.begin(); i < num_entities_removed; ++it, ++i) {
                    aliens.remove(*it);
                    shields.remove(*it);
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
            if (player_dead && none_active(explosions)) {
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
                dead_pause = SDL_GetTicks();
                //sound.play_player_dead();
                //status.erase_player_ship(player_life, get_image("ship.png"));
                player_rebirth();
                //sound.play_bonus();
                //sound.play_bg(alien_count);
                last_loop_time += SDL_GetTicks() - dead_pause;
                last_bonus_launch = last_alien_shot = SDL_GetTicks();
                // erase player and alien shots to give player a chance to continue
                for (vIter it = alien_shots.begin(); it != alien_shots.end(); ++it) {
                    (*it)->deactivate();
                }
                for (vIter it = player_shots.begin(); it != player_shots.end(); ++it) {
                    (*it)->deactivate();
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
                if (none_active(explosions) && none_active(player_shots) && none_active(alien_shots)) {
                    wave_cleanup();
                    //sound.wait_for_all_to_finish();
                    //sound.play_end_wave();
                    return;
                }
            }

            // run alien logic if neccessary
            if (logic_this_loop) {
                for (Iter it = aliens.begin(); it != aliens.end(); ++it) {
                    (*it)->do_alien_logic();
                }
                logic_this_loop = false;
            }

            // draw everything
            if (player->is_active())
                player->draw();
            if (bonus->is_active())
                bonus->draw();
            for (Iter it = aliens.begin(); it != aliens.end(); ++it) {
                    (*it)->draw();
            }
            for (vIter it = player_shots.begin(); it != player_shots.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->draw();
                }
            }
            for (vIter it = alien_shots.begin(); it != alien_shots.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->draw();
                }
            }
            for (vIter it = explosions.begin(); it != explosions.end(); ++it) {
                if ((*it)->is_active()) {
                    (*it)->draw();
                }
            }
            for (Iter it = shields.begin(); it != shields.end(); ++it) {
                (*it)->draw();
            }
            // update screen
            for (int i = 0; i < screen_updates; ++i) {
                SDL_BlitSurface(blits[i].img, blits[i].src_rect, screen, blits[i].dst_rect);
            }
            SDL_UpdateRects(screen, screen_updates, dst);
            screen_updates = 0;
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
        typedef std::list<boost::shared_ptr<GameEntities::GameEntity> >::iterator Iter;
        for (Iter it = shields.begin(); it != shields.end(); ++it) {
                (*it)->cleanup_draw();
        }
        for (Iter it = aliens.begin(); it != aliens.end(); ++it) {
                (*it)->cleanup_draw();
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
        typedef std::list<boost::shared_ptr<GameEntities::GameEntity> >::iterator Iter;
        for (Iter it = shields.begin(); it != shields.end(); ++it) {
                (*it)->cleanup_draw();
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
        begin_pause = SDL_GetTicks();
        while (1) {
            keys = SDL_GetKeyState(NULL);
            while (SDL_PollEvent(&event)) {
                if (keys[SDLK_p]) {
                    last_loop_time +=  SDL_GetTicks() - begin_pause;
                    last_bonus_launch = last_alien_shot = SDL_GetTicks();
                    //sound.play_bonus();
                    return;
                }
            }
        }
    }
    void Game::fire_shot()
    {
        // check that player has waited long enough to fire
        if (SDL_GetTicks() - last_shot < player_shot_delay) {
            return;
        }
        // record time and fire
        last_shot = SDL_GetTicks();
        player_shots[player_shot_counter]->init_x(player->get_x()+player_init_x_shot_pos);
        player_shots[player_shot_counter]->init_y(player->get_y()-player_init_y_shot_pos);
        player_shots[player_shot_counter]->set_hit(false);
        player_shots[player_shot_counter]->activate();
        if (++player_shot_counter == num_player_shots) {
            player_shot_counter = 0;
        }
        //sound.play_shot();
    }
    void Game::launch_bonus_ship()
    {
        static const int top = 60;
        if (SDL_GetTicks() - last_bonus_launch < bonus_launch_delay) {
            return;
        }
        last_bonus_launch = SDL_GetTicks();
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
    }
    void Game::alien_fire()
    {
        // check that aliens have waited long enough to fire
        if (SDL_GetTicks() - last_alien_shot < alien_shot_delay) {
            return;
        }
        // record time and fire
        last_alien_shot = SDL_GetTicks();
        static int alien_to_fire = 0;
        ++alien_to_fire;
        typedef std::list<boost::shared_ptr<GameEntities::GameEntity> >::iterator Iter;
        for (Iter alien = aliens.begin(); alien != aliens.end(); ++alien) {
            if ((*alien)->is_active() && (*alien)->get_fire_chance() == alien_to_fire) {
                alien_shots[alien_shot_counter]->init_x((*alien)->get_x()+alien_init_x_shot_pos);
                alien_shots[alien_shot_counter]->init_y((*alien)->get_y()+alien_init_y_shot_pos);
                alien_shots[alien_shot_counter]->set_hit(false);
                alien_shots[alien_shot_counter]->activate();
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
        typedef std::list<boost::shared_ptr<GameEntities::GameEntity> >::iterator Iter;
        for (Iter it = aliens.begin(); it != aliens.end(); ++it) {
            if ((*it)->get_pos() == pos - 12) {
                (*it)->activate();
            }
            (*it)->increase_x_speed(1052);
            switch (alien_count) {
                case 4:
                    (*it)->increase_x_speed(1178);
                    //sound.halt_bg(alien_count);
                    break;
                case 3:
                    //sound.halt_bg(alien_count);
                    break;
                case 2:
                    (*it)->increase_x_speed(1178);
                    //sound.halt_bg(alien_count);
                    break;
                case 1:
                    (*it)->increase_x_speed(1178);
                    //sound.halt_bg(alien_count);
                    break;
                default:
                    break;
            }
        }
    }
    int Game::none_active(const std::vector<boost::shared_ptr<GameEntities::GameEntity> >& vec)
    {
        typedef std::vector<boost::shared_ptr<GameEntities::GameEntity> >::const_iterator vIter;
        vIter it = vec.begin();
        while (it != vec.end()) {
            if ((*it)->is_active()) {
                return 0;
            }
            ++it;
        }
        return 1;
    }
    void Game::explode(fixed x, fixed y, Uint32 duration)
    {
        explosions[explosion_counter]->init_x(x);
        explosions[explosion_counter]->init_y(y);
        explosions[explosion_counter]->set_explosion(duration);
        if (++explosion_counter == num_explosions) {
            explosion_counter = 0;
        }
    }
    Game::Game() : ui(&sound, this, 0), player_life(0), sdl_quit_event(false)
    {
        set_video_mode(0);
        load_images();
        wave_background = image_cache["wave_background.png"];
        background = image_cache["background.png"];
        ui_header = image_cache["ui_header.png"];
        ui_points = image_cache["ui_points.png"];
    }
    void Game::set_video_mode(int fullscreen)
    {
        if (fullscreen) {
            screen = SDL_SetVideoMode(screen_w, screen_h, 16, SDL_SWSURFACE|SDL_FULLSCREEN);
        } else {
            screen = SDL_SetVideoMode(screen_w, screen_h, 16, SDL_SWSURFACE);
        }
        if (screen == NULL) {
            std::cerr << "Unable to set video mode: " << SDL_GetError() << '\n';
            throw "SDL: Error setting video mode.";
        }
    }
    Game::~Game()
    {
        free_images();
    }
    SDL_Surface* Game::get_image(const char* image)
    {
        return image_cache[image];
    }
    void Game::free_images()
    {
        typedef std::map<const char*, SDL_Surface*>::iterator mIter;
        for (mIter it = image_cache.begin(); it != image_cache.end(); ++it) {
            SDL_FreeSurface(it->second);
        }
    }
    void Game::load_images()
    {
        // a list of image files are loaded into a map and keyed by filename
        std::vector<const char*> image_list = boost::assign::list_of("ship.png") ("shot.png")
            ("alien-1-1.png")  ("alien-1-2.png")  ("alien-1-3.png")  ("alien-1-4.png")
            ("alien-2-1.png")  ("alien-2-2.png")  ("alien-2-3.png")  ("alien-2-4.png")
            ("alien-3-1.png")  ("alien-3-2.png")  ("alien-3-3.png")  ("alien-3-4.png")
            ("bonus-1-1.png")  ("bonus-1-2.png")  ("wave_background.png") ("background.png")
            ("explosion.png") ("shield_piece.png") ("bonus-2-1.png") ("bonus-2-2.png")
            ("ui_header.png") ("ui_points.png")
        ;

        typedef std::vector<const char*>::const_iterator Iter;
        for (Iter it = image_list.begin(); it != image_list.end(); ++it) {
            std::string path = datadir + *it;
            SDL_Surface* temp = IMG_Load(path.c_str());
            if (temp == NULL) {
                free_images();
                std::ostringstream s;
                s << "Unable to load image file: " << *it << '\n';
                throw s.str();
            }
            SDL_Surface* image = SDL_DisplayFormatAlpha(temp);
            image_cache[*it] = image;
            SDL_FreeSurface(temp);
        }
    }

}
