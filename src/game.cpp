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

using Graphics::Images;
using Graphics::Screen;

using GameEntities::GameEntityTypeProperties;
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

// Taken from image file dimensions.
#define PLAYER_WIDTH             43
#define PLAYER_HEIGHT            25
#define ALIEN_WIDTH              45
#define ALIEN_HEIGHT             30
#define BONUS_SHIP_WIDTH         60
#define BONUS_SHIP_HEIGHT        26
#define SMALL_BONUS_SHIP_WIDTH   45
#define SMALL_BONUS_SHIP_HEIGHT  20
#define SHOT_WIDTH                5
#define SHOT_HEIGHT              16

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

namespace {
    // Put all the data arrays needed by Game into a separate struct, so the
    // amount of memory required can be easily obtained using sizeof().
    struct GameData {
        Alien alien_array[NUM_ALIENS];
        bool shield_array[NUM_SHIELDS];

        Shot player_shot_array[num_player_shots];
        Shot alien_shot_array[MAX_NUM_ALIEN_SHOTS];
        Explosion explosion_array[num_explosions];

        int direction_array[random_list_len];
        int bonus_select_array[random_list_len];
        int launch_delay_array[random_list_len];

        GameEntity shield_group_array[NUM_SHIELD_GROUPS];
    };

    // Initializes a shield entity given its index and state.
    void make_shield(ShieldPiece* shield, int index, bool active) {
        int j     = index / (NUM_SHIELDS / NUM_SHIELD_GROUPS);
        int j_mod = index % (NUM_SHIELDS / NUM_SHIELD_GROUPS);
        int k     = j_mod / SHIELD_GROUP_HEIGHT;
        int k_mod = j_mod % SHIELD_GROUP_HEIGHT;
        int i     = k_mod / SHIELD_GROUP_WIDTH;
        int i_mod = k_mod % SHIELD_GROUP_WIDTH;

        int x = (j * SHIELD_GROUP_X_SPACING) + SHIELD_X_OFFSET +
                (i * SHIELD_PIECE_SIZE);
        int y = SHIELD_Y_OFFSET + SHIELD_PIECE_SIZE * k;

        shield->ShieldPiece_init(x, y, 0, 0, active);
    }
}

namespace Game {

    void Game::game_control()
    {
        printf("Memory needed for game data: %u bytes\n", sizeof(GameData));
        printf("Memory needed for one game entity: %u bytes\n",
               sizeof(GameEntity));
        printf("Memory needed for game entity type properties: %u bytes\n",
               sizeof(GameEntityTypeProperties) * NUM_GAME_ENTITY_TYPES);

        // Define arrays statically, so they can be freed when the game loop exits.
        GameData data;

        // Populate the game data pointers.
        aliens = data.alien_array;
        shields = data.shield_array;

        player_shots = data.player_shot_array;
        alien_shots = data.alien_shot_array;
        explosions = data.explosion_array;

        direction = data.direction_array;
        bonus_select = data.bonus_select_array;
        launch_delay = data.launch_delay_array;

        shield_groups = data.shield_group_array;

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
        last_shot = 0;
        player_shot_counter = alien_shot_counter = explosion_counter = 0;
        factory();
        // output wave message and status display
        //ui.wave_msg(++wave);
        //sound.play_start_wave();
        screen.update();
        //status.blit_wave(wave);
        //status.blit_score(score);
        //status.blit_lives(player_life);
        //status.blit_player_ships(player_life, images.get_image_index("ship.png"));
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
                                               0, active, alien_count+1,
                                               RAND(rand_max));
                ++alien_count;
            }
        }
        // create alien shots
        for (int i= 0; i < num_alien_shots; ++i) {
            alien_shots[i].Shot_init(0, 0, 0, alien_shot_speed, false);
        }
    }
    void Game::factory()
    {
        // create the player ship and place it in the center of the screen
        player->Player_init(player_center, player_top, 0, 0, true);

        // create bonus ship and small bonus ship
        bonus->BonusShip_init(false, 0, 0, 0, 0, false);
        sbonus->BonusShip_init(true, 0, 0, 0, 0, false);

/*
                    int x = (j * SHIELD_GROUP_X_SPACING) +
                            SHIELD_X_OFFSET + ((k == 0) ? 20 : 0) +
                            (i * SHIELD_PIECE_SIZE);
                    int y = SHIELD_Y_OFFSET + SHIELD_PIECE_SIZE * k;
*/
        // create the shields
        int num_shields = 0;
        memset(shields, 0, NUM_SHIELDS * sizeof(shields[0]));
        for (int j = 0; j < NUM_SHIELD_GROUPS; ++j) {
            for (int k = 0; k < SHIELD_GROUP_HEIGHT; ++k) {
                for (int i = 0; i < SHIELD_GROUP_WIDTH; ++i) {
                    //shields[num_shields++].ShieldPiece_init(x, y, 0, 0, true);
                    if ((k == 0) && (i == 0 || i == SHIELD_GROUP_WIDTH - 1))
                        continue;
                    shields[num_shields++] = 1;
                }
            }

            shield_groups[j].init(GAME_ENTITY_SHIELD_GROUP,
                                  j * SHIELD_GROUP_X_SPACING + SHIELD_X_OFFSET,
                                  SHIELD_Y_OFFSET, 0, 0, true);
        }
        for (int type = 0; type < NUM_GAME_ENTITY_TYPES; ++type) {
            GameEntities::GameEntityTypeProperties prop;
            memset(&prop, 0, sizeof(prop));

            switch(type) {
            case GAME_ENTITY_PLAYER:
                prop.images[0] = images.get_image_index("ship.png");

                prop.w = PLAYER_WIDTH;
                prop.h = PLAYER_HEIGHT;
                prop.coll_w = int (PLAYER_WIDTH * 0.9);
                prop.coll_h = int (PLAYER_HEIGHT * 0.8);
                prop.right_limit = screen_w - PLAYER_WIDTH;
                break;

            case GAME_ENTITY_ALIEN:
                prop.images[0] = images.get_image_index("alien-1-1.png");
                prop.images[1] = images.get_image_index("alien-1-2.png");
                prop.images[2] = images.get_image_index("alien-1-3.png");
                prop.images[3] = images.get_image_index("alien-1-4.png");
                prop.images[4] = images.get_image_index("alien-1-3.png");
                prop.images[5] = images.get_image_index("alien-1-2.png");

                prop.points = 25;
                prop.frame_duration = 225;
                prop.right_limit = screen_w - ALIEN_WIDTH;
                prop.bottom_limit = 530;
                prop.w = ALIEN_WIDTH;
                prop.h = ALIEN_HEIGHT;
                prop.coll_w = ALIEN_WIDTH;
                prop.coll_h = int (ALIEN_HEIGHT * 0.8);
                break;
            case GAME_ENTITY_ALIEN2:
                prop.images[0] = images.get_image_index("alien-2-1.png");
                prop.images[1] = images.get_image_index("alien-2-2.png");
                prop.images[2] = images.get_image_index("alien-2-3.png");
                prop.images[3] = images.get_image_index("alien-2-4.png");
                prop.images[4] = images.get_image_index("alien-2-3.png");
                prop.images[5] = images.get_image_index("alien-2-2.png");

                prop.points = 50;
                prop.frame_duration = 225;
                prop.right_limit = screen_w - ALIEN_WIDTH;
                prop.bottom_limit = 530;
                prop.w = ALIEN_WIDTH;
                prop.h = ALIEN_HEIGHT;
                prop.coll_w = ALIEN_WIDTH;
                prop.coll_h = int (ALIEN_HEIGHT * 0.8);
                break;
            case GAME_ENTITY_ALIEN3:
                prop.images[0] = images.get_image_index("alien-3-1.png");
                prop.images[1] = images.get_image_index("alien-3-2.png");
                prop.images[2] = images.get_image_index("alien-3-3.png");
                prop.images[3] = images.get_image_index("alien-3-4.png");
                prop.images[4] = images.get_image_index("alien-3-3.png");
                prop.images[5] = images.get_image_index("alien-3-2.png");

                prop.points = 100;
                prop.frame_duration = 225;
                prop.right_limit = screen_w - ALIEN_WIDTH;
                prop.bottom_limit = 530;

                prop.w = ALIEN_WIDTH;
                prop.h = ALIEN_HEIGHT;
                prop.coll_w = int (ALIEN_WIDTH * 0.8);
                prop.coll_h = int (ALIEN_HEIGHT * 0.8);

                break;
            case GAME_ENTITY_BONUS_SHIP:
                prop.images[0] = images.get_image_index("bonus-1-1.png");
                prop.images[1] = images.get_image_index("bonus-1-2.png");

                prop.points = 1000;
                prop.frame_duration = 55;
                prop.w = BONUS_SHIP_WIDTH;
                prop.coll_w = int (BONUS_SHIP_WIDTH * 0.9);
                prop.coll_h = prop.h = BONUS_SHIP_HEIGHT;
                break;
            case GAME_ENTITY_SMALL_BONUS_SHIP:
                prop.images[0] = images.get_image_index("bonus-2-1.png");
                prop.images[1] = images.get_image_index("bonus-2-2.png");

                prop.points = 5000;
                prop.frame_duration = 55;
                prop.w = SMALL_BONUS_SHIP_WIDTH;
                prop.coll_w = int (SMALL_BONUS_SHIP_WIDTH * 0.9);
                prop.coll_h = prop.h = SMALL_BONUS_SHIP_HEIGHT;
                break;
            case GAME_ENTITY_SHOT:
                prop.images[0] = images.get_image_index("shot.png");

                prop.coll_w = prop.w = SHOT_WIDTH;
                prop.h = SHOT_HEIGHT;
                prop.coll_h = int (SHOT_HEIGHT * 0.7);

                break;
            case GAME_ENTITY_SHIELD_PIECE:
                prop.images[0] = images.get_image_index("shield_piece.png");

                prop.coll_w = prop.w = SHIELD_PIECE_SIZE;
                prop.coll_h = int (SHIELD_PIECE_SIZE * 0.9);
                break;
            case GAME_ENTITY_EXPLOSION:
                prop.images[0] = images.get_image_index("explosion.png");
                break;
            case GAME_ENTITY_SHIELD_GROUP:
                prop.w = prop.coll_w = SHIELD_GROUP_WIDTH * SHIELD_PIECE_SIZE;
                prop.h = prop.coll_h = SHIELD_GROUP_HEIGHT * SHIELD_PIECE_SIZE;
                break;
            default:
                continue;
            }

            prop.coll_x_offset = (prop.w - prop.coll_w) / 2;
            prop.coll_y_offset = (prop.h - prop.coll_h) / 2;

            GameEntity::set_type_property(type, prop);
        }

        // create explosions and player shots
        for (int i= 0; i < num_explosions; ++i) {
            explosions[i].Explosion_init(0, 0, 0, 0, false);
        }
        for (int i= 0; i < num_player_shots; ++i) {
            player_shots[i].Shot_init(0, 0, 0, shot_speed, false);
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
        System::KeyState keys;
        int reloading = 0;
        last_bonus_launch = last_alien_shot = last_loop_time = System::get_ticks();

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
            delta = System::get_ticks() - last_loop_time;
            last_loop_time = System::get_ticks();

            // background track
            //sound.play_bg(alien_count);

#ifdef FRAME_COUNTER
            // frame counter
            uint32_t last_fps_time;
            int fps;
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
            keys = System::get_key_state();
            if (keys.quit) {
                //sound.halt_all_sounds();
                player_dead = true;
                player_life = 0;
                return;
            }
            if (keys.pause) {
                pause();
            }

            // player attempt to fire
            if (!reloading) {
                if (keys.fire) {
                    fire_shot();
                }
            }
            reloading = keys.fire;


            // set player direction based on key input
            player->set_x_velocity(0);
            if (keys.left && !keys.right) {
                player->set_x_velocity(-player_speed);
            }
            if (keys.right && !keys.left) {
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
                    //ShieldPiece* shield = &shields[i];
                    ShieldPiece shield;
                    make_shield(&shield, i, shields[i]);
                    if (shield.is_alive() && shield.collides_with(shot)) {
                        shot->shot_shield_collision(&shield);
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
                    ShieldPiece shield;
                    make_shield(&shield, i, shields[i]);
                    if (shield.is_alive() && shield.collides_with(shot)) {
                        shot->shot_shield_collision(&shield);
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
                    ShieldPiece shield;
                    make_shield(&shield, i, shields[i]);
                    if (shield.is_alive() && shield.collides_with(alien)) {
                        alien->alien_shield_collision(&shield);
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
                dead_pause = System::get_ticks();
                //sound.play_player_dead();
                //status.erase_player_ship(player_life, images.get_image_index("ship.png"));
                player_rebirth();
                //sound.play_bonus();
                //sound.play_bg(alien_count);
                last_loop_time += System::get_ticks() - dead_pause;
                last_bonus_launch = last_alien_shot = System::get_ticks();
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
                ShieldPiece shield;
                make_shield(&shield, i, shields[i]);
                if (shields[i])
                    shield.draw();
            }
            screen.update();

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
            ShieldPiece shield;
            make_shield(&shield, i, shields[i]);
            if (shields[i])
                shield.cleanup_draw();
        }
        for (int i = 0; i < NUM_ALIENS; ++i) {
            aliens[i].cleanup_draw();
        }
        screen.update();
        //sound.play_player_rebirth();
    }
    void Game::wave_cleanup()
    {
        // redraw all that should remain
        if (player->is_active())
            player->cleanup_draw();
        for (int i = 0; i < NUM_SHIELDS; ++i) {
            ShieldPiece shield;
            make_shield(&shield, i, shields[i]);
            if (shields[i])
                shield.cleanup_draw();
        }
        screen.update();
    }
    void Game::pause()
    {
        uint32_t begin_pause;
        //sound.halt_bonus();
        //sound.halt_bg(alien_count);
        begin_pause = System::get_ticks();
        while (1) {
            System::KeyState keys = System::get_key_state();
            if (keys.pause) {
                last_loop_time +=  System::get_ticks() - begin_pause;
                last_bonus_launch = last_alien_shot = System::get_ticks();
                //sound.play_bonus();
                return;
            }
        }
    }
    void Game::fire_shot()
    {
        // check that player has waited long enough to fire
        if (System::get_ticks() - last_shot < player_shot_delay) {
            return;
        }
        // record time and fire
        last_shot = System::get_ticks();
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
        if (System::get_ticks() - last_bonus_launch < bonus_launch_delay) {
            return;
        }
        last_bonus_launch = System::get_ticks();
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
        if (System::get_ticks() - last_alien_shot < alien_shot_delay) {
            return;
        }
        // record time and fire
        last_alien_shot = System::get_ticks();
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
            //status.blit_player_ships(player_life, images.get_image_index("ship.png"));
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
    Game::Game(Screen* screen_ptr, Images* images_ptr) :
                   // ui(&sound, this, 0),
                   screen(*screen_ptr),
                   images(*images_ptr),
                   player_life(0),
                   player(NULL),
                   bonus(NULL),
                   sbonus(NULL),
                   rbonus(NULL)
    {
        screen.init();
        screen.set_video_mode(false);
        images.load_images(image_list);
        screen.set_image_lib(&images);

        GameEntity::set_game(this);
        GameEntity::set_screen(&screen);
    }
    Game::~Game()
    {
        GameEntity::set_game(NULL);
    }
}
