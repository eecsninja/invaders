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

#include "game_defs.h"
#include "game_entity.h"
#include "printf.h"
#include "screen.h"
#include "shields.h"
#include "starfield.h"
#include "system.h"

extern const char* datadir;

//#define FRAME_COUNTER

using Graphics::Screen;

using GameEntities::GameEntityTypeProperties;
using GameEntities::GameEntity;
using GameEntities::Player;
using GameEntities::Alien;
using GameEntities::BonusShip;
using GameEntities::Explosion;
using GameEntities::Shot;

using Game::ShieldPiece;
using Game::ShieldGroupTiles;

#ifdef EVENT_COUNTER
EventCounter event_counter;
#endif

#define PAUSE_COOLDOWN_TIME      100

#define max(a, b)   ( ((a)>=(b)) ? (a) : (b) )
#define min(a, b)   ( ((b)>=(a)) ? (a) : (b) )

#define STARFIELD_LAYER_INDEX    0
#define STARFIELD2_LAYER_INDEX   1
#define SHIELD_LAYER_INDEX       3

namespace {

  // Determines which type of alien is in each row.
  const uint8_t kAlienTypesByRow[ALIEN_ARRAY_HEIGHT] = {
    GAME_ENTITY_ALIEN3,
    GAME_ENTITY_ALIEN2,
    GAME_ENTITY_ALIEN2,
    GAME_ENTITY_ALIEN,
    GAME_ENTITY_ALIEN,
  };

  // Keep count of per-type index offsets in each row.
  uint8_t per_type_index_offsets[ALIEN_ARRAY_HEIGHT] = { 0, 0, 0, 0, 0 };

  inline int get_alien_type_by_row(int row) {
    return kAlienTypesByRow[row];
  }

    // Put all the data arrays needed by Game into a separate struct, so the
    // amount of memory required can be easily obtained using sizeof().
    struct GameData {
        Game::ReducedAlien alien_array[NUM_ALIENS];
        Alien reference;
        uint8_t num_aliens_per_col[ALIEN_ARRAY_WIDTH];

        ShieldPiece shield_array[NUM_SHIELDS];

        Shot player_shot_array[num_player_shots];
        Shot alien_shot_array[MAX_NUM_ALIEN_SHOTS];
        Explosion explosion_array[num_explosions];

        int direction_array[random_list_len];
        int bonus_select_array[random_list_len];
        int launch_delay_array[random_list_len];

        GameEntity shield_group_array[NUM_SHIELD_GROUPS];
        ShieldGroupTiles shield_group_tiles_array[NUM_SHIELD_GROUPS];
    };

    // Initializes a shield piece entity given its index and state.
    void make_shield(const ShieldPiece& piece, GameEntity* object) {
        int x = (piece.group * SHIELD_GROUP_X_SPACING) +
                SHIELD_X_OFFSET +
                (piece.x * SHIELD_PIECE_SIZE);
        int y = SHIELD_Y_OFFSET + SHIELD_PIECE_SIZE * piece.y;
        object->ShieldPiece_init(0, x, y, piece.intact);
    }

    fixed increase_speed(fixed speed, fixed32 increase) {
        // Multiplying fixed and fixed32 results in 12 bits after the point.
        // FIXED32_TO_INT() converts it back into 4-bit post-point precision.
        fixed new_speed = FIXED32_TO_INT(speed * increase);
        if (new_speed != speed || speed == 0)
            return new_speed;
        // If the increase was so small that speed was not updated, inc/dec the
        // speed by 1 so it is still updated.
        if (speed > 0)
            return speed + 1;
        else
            return speed - 1;
     }

    // Generates a full alien object.
    void make_alien(const Game::ReducedAlien& alien, const Alien* reference,
                    Alien* new_alien) {
        uint8_t type = get_alien_type_by_row(alien.row);
        uint8_t index = per_type_index_offsets[alien.row] + alien.col;
        int x = reference->get_x() + alien.col * ALIEN_STEP_X;
        int y = reference->get_y() + alien.row * ALIEN_STEP_Y;
        new_alien->Alien_init(type, index, x, y,
                              alien.is_active(),
                              alien.get_fire_chance());
        if (!alien.is_alive() && new_alien->is_alive()) {
            new_alien->kill();
        }
        new_alien->set_image_num(reference->get_image_num());
    }

}  // namespace

namespace Game {

    void Game::game_control()
    {
        printf_P("Memory needed for game data: %u bytes\n",
                 sizeof(GameData) + sizeof(Game));
        printf_P("Memory needed for one game entity: %u bytes\n",
                 sizeof(GameEntity));
        printf_P("Memory needed for game entity type properties: %u bytes\n",
                 sizeof(GameEntityTypeProperties) * NUM_GAME_ENTITY_TYPES);

        // Define arrays statically, so they can be freed when the game loop exits.
        GameData data;

        printf_P("Game data allocated at 0x%x (%u bytes)\n", &data, &data);

        // Populate the game data pointers.
        aliens = data.alien_array;
        reference_alien = &data.reference;
        num_aliens_per_col = data.num_aliens_per_col;
        shields = data.shield_array;

        player_shots = data.player_shot_array;
        alien_shots = data.alien_shot_array;
        explosions = data.explosion_array;

        direction = data.direction_array;
        bonus_select = data.bonus_select_array;
        launch_delay = data.launch_delay_array;

        shield_groups = data.shield_group_array;
        shield_group_tiles = data.shield_group_tiles_array;

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
        current_alien_speed = INT_TO_FIXED(ALIEN_BASE_SPEED);
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
    void Game::init_aliens(int rand_max)
    {
        // Keep count of both the total number of aliens and the number of each
        // type of alien.
        alien_count = 0;
        uint8_t alien_type_counts[NUM_GAME_ENTITY_TYPES];
        memset(alien_type_counts, 0, sizeof(alien_type_counts));
        // Create a formation of aliens.
        for (int row = 0; row < ALIEN_ARRAY_HEIGHT; ++row) {
            int type = get_alien_type_by_row(row);
            // For the first alien in each row, store its index relative to
            // other aliens of its own class.
            per_type_index_offsets[row] = alien_type_counts[type];
            for (int col = 0; col < ALIEN_ARRAY_WIDTH; ++col) {
                // Populate the alien count for each column.  This is redundant
                // because it goes through the same initialization
                // |ALIEN_ARRAY_HEIGHT| times, but it saves code space by not
                // having a separate for loop from 0 to |ALIEN_ARRAY_WIDTH - 1|.
                num_aliens_per_col[col] = ALIEN_ARRAY_HEIGHT;

                // initialize the bottom row of aliens to fire
                bool active = (row == ALIEN_ARRAY_HEIGHT - 1);
                int index = alien_type_counts[type]++;

                // Instantiate an alien.
                Alien temp_alien;
                temp_alien.Alien_init(type, index, ALIEN_BASE_X, ALIEN_BASE_Y,
                                      active, rand() % (rand_max + 1));

                // Convert it to a reduced alien.
                ReducedAlien& alien = aliens[alien_count];
                alien.alive = temp_alien.is_alive();
                alien.active = temp_alien.is_active();
                alien.dirty = temp_alien.is_dirty();
                alien.fire_chance = temp_alien.get_fire_chance();
                alien.row = row;
                alien.col = col;

                // If this is the first alien, save it as a reference.
                if (alien_count == 0) {
                  *reference_alien = temp_alien;
                }
                ++alien_count;
            }
        }
        // create alien shots
        for (int i = 0; i < num_alien_shots; ++i) {
            alien_shots[i].Shot_init(num_player_shots + i, 0, 0, false);
        }
    }
    void Game::factory()
    {
        // create the player ship and place it in the center of the screen
        player->Player_init(player_center, player_top, true);
        current_player_speed = 0;

        // create bonus ship and small bonus ship
        bonus->BonusShip_init(false, 0, 0, false);
        sbonus->BonusShip_init(true, 0, 0, false);
        current_bonus_speed = 0;

        // create the shields
        int num_shields = 0;
        memset(shields, 0, NUM_SHIELDS * sizeof(shields[0]));
        for (int j = 0; j < NUM_SHIELD_GROUPS; ++j) {
            for (int k = 0; k < SHIELD_GROUP_HEIGHT; ++k) {
                for (int i = 0; i < SHIELD_GROUP_WIDTH; ++i) {
                    ShieldPiece& shield = shields[num_shields++];
                    //shields[num_shields++].ShieldPiece_init(x, y, 0, 0, true);
                    shield.intact = true;
                    if ((k == 0) && (i == 0 || i == SHIELD_GROUP_WIDTH - 1))
                        shield.intact = false;
                    shield.group = j;
                    shield.x = i;
                    shield.y = k;

                    shield_group_tiles[j].update_shield_piece(shield);
                }
            }

            shield_groups[j].init(GAME_ENTITY_SHIELD_GROUP, j,
                                  j * SHIELD_GROUP_X_SPACING + SHIELD_X_OFFSET,
                                  SHIELD_Y_OFFSET, true);
        }
        for (int type = 0; type < NUM_GAME_ENTITY_TYPES; ++type) {
            GameEntities::GameEntityTypeProperties prop;
            memset(&prop, 0, sizeof(prop));

            switch(type) {
            case GAME_ENTITY_PLAYER:
                prop.images[0] = 0;

                prop.w = PLAYER_WIDTH;
                prop.h = PLAYER_HEIGHT;
                prop.coll_w = int (PLAYER_WIDTH * 0.9);
                prop.coll_h = int (PLAYER_HEIGHT * 0.8);
                prop.right_limit = screen_w - PLAYER_WIDTH;

                prop.sprite_w = PLAYER_SPRITE_WIDTH;
                prop.sprite_h = PLAYER_SPRITE_HEIGHT;
                break;

            case GAME_ENTITY_ALIEN:
                prop.images[0] = 0;
                prop.images[1] = 1;
                prop.images[2] = 2;
                prop.images[3] = 3;
                prop.images[4] = 2;
                prop.images[5] = 1;

                prop.points = 25;
                prop.frame_duration = 225;
                prop.right_limit = screen_w - ALIEN_WIDTH;
                prop.bottom_limit = player_top - ALIEN_HEIGHT / 3;
                prop.w = ALIEN_WIDTH;
                prop.h = ALIEN_HEIGHT;
                prop.coll_w = ALIEN_WIDTH;
                prop.coll_h = int (ALIEN_HEIGHT * 0.8);

                prop.sprite_w = ALIEN_SPRITE_WIDTH;
                prop.sprite_h = ALIEN_SPRITE_HEIGHT;
                break;
            case GAME_ENTITY_ALIEN2:
                prop.images[0] = 0;
                prop.images[1] = 1;
                prop.images[2] = 2;
                prop.images[3] = 3;
                prop.images[4] = 2;
                prop.images[5] = 1;

                prop.points = 50;
                prop.frame_duration = 225;
                prop.right_limit = screen_w - ALIEN_WIDTH;
                prop.bottom_limit = player_top - ALIEN_HEIGHT / 3;
                prop.w = ALIEN_WIDTH;
                prop.h = ALIEN_HEIGHT;
                prop.coll_w = ALIEN_WIDTH;
                prop.coll_h = int (ALIEN_HEIGHT * 0.8);

                prop.sprite_w = ALIEN_SPRITE_WIDTH;
                prop.sprite_h = ALIEN_SPRITE_HEIGHT;
                break;
            case GAME_ENTITY_ALIEN3:
                prop.images[0] = 0;
                prop.images[1] = 1;
                prop.images[2] = 2;
                prop.images[3] = 3;
                prop.images[4] = 2;
                prop.images[5] = 1;

                prop.points = 100;
                prop.frame_duration = 225;
                prop.right_limit = screen_w - ALIEN_WIDTH;
                prop.bottom_limit = player_top - ALIEN_HEIGHT / 3;

                prop.w = ALIEN_WIDTH;
                prop.h = ALIEN_HEIGHT;
                prop.coll_w = int (ALIEN_WIDTH * 0.8);
                prop.coll_h = int (ALIEN_HEIGHT * 0.8);

                prop.sprite_w = ALIEN_SPRITE_WIDTH;
                prop.sprite_h = ALIEN_SPRITE_HEIGHT;
                break;
            case GAME_ENTITY_BONUS_SHIP:
                prop.images[0] = 0;
                prop.images[1] = 1;

                prop.points = 1000;
                prop.frame_duration = 55;
                prop.w = BONUS_SHIP_WIDTH;
                prop.coll_w = int (BONUS_SHIP_WIDTH * 0.9);
                prop.coll_h = prop.h = BONUS_SHIP_HEIGHT;

                prop.sprite_w = BONUS_SHIP_SPRITE_WIDTH;
                prop.sprite_h = BONUS_SHIP_SPRITE_HEIGHT;
                break;
            case GAME_ENTITY_SMALL_BONUS_SHIP:
                prop.images[0] = 0;
                prop.images[1] = 1;

                prop.points = 5000;
                prop.frame_duration = 55;
                prop.w = SMALL_BONUS_SHIP_WIDTH;
                prop.coll_w = int (SMALL_BONUS_SHIP_WIDTH * 0.9);
                prop.coll_h = prop.h = SMALL_BONUS_SHIP_HEIGHT;

                prop.sprite_w = SMALL_BONUS_SHIP_SPRITE_WIDTH;
                prop.sprite_h = SMALL_BONUS_SHIP_SPRITE_HEIGHT;
                break;
            case GAME_ENTITY_SHOT:
                prop.images[0] = 0;

                prop.coll_w = prop.w = SHOT_WIDTH;
                prop.h = SHOT_HEIGHT;
                prop.coll_h = int (SHOT_HEIGHT * 0.7);

                prop.sprite_w = SHOT_SPRITE_WIDTH;
                prop.sprite_h = SHOT_SPRITE_HEIGHT;
                break;
            case GAME_ENTITY_SHIELD_PIECE:
                prop.images[0] = 0;

                prop.coll_w = prop.w = SHIELD_PIECE_SIZE;
                prop.h = SHIELD_PIECE_SIZE;
                prop.coll_h = int (SHIELD_PIECE_SIZE * 0.9);
                break;
            case GAME_ENTITY_EXPLOSION:
                prop.images[0] = 0;
                prop.sprite_w = EXPLOSION_SPRITE_SIZE;
                prop.sprite_h = EXPLOSION_SPRITE_SIZE;
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

        // Allocate sprites for each type of entity.
        int num_objects_per_type[NUM_GAME_ENTITY_TYPES];
        memset(num_objects_per_type, 0, sizeof(num_objects_per_type));
        num_objects_per_type[GAME_ENTITY_PLAYER] = 1;
        num_objects_per_type[GAME_ENTITY_ALIEN] = 2 * ALIEN_ARRAY_WIDTH;
        num_objects_per_type[GAME_ENTITY_ALIEN2] = 2 * ALIEN_ARRAY_WIDTH;
        num_objects_per_type[GAME_ENTITY_ALIEN3] = ALIEN_ARRAY_WIDTH;
        num_objects_per_type[GAME_ENTITY_BONUS_SHIP] = 1;
        num_objects_per_type[GAME_ENTITY_SMALL_BONUS_SHIP] = 1;
        num_objects_per_type[GAME_ENTITY_SHOT]
            = num_player_shots + MAX_NUM_ALIEN_SHOTS;
        num_objects_per_type[GAME_ENTITY_SHIELD_PIECE] = 0;
        num_objects_per_type[GAME_ENTITY_EXPLOSION] = num_explosions;
        screen.allocate_sprites(num_objects_per_type);

        // Set up tile layer for drawing shield pieces.
        uint16_t shield_data_offset =
                screen.get_image_offset(GAME_ENTITY_SHIELD_PIECE);
        screen.setup_tile_layer(SHIELD_LAYER_INDEX, true, 0,
                                shield_data_offset, 0xff);

        screen.scroll_tile_layer(SHIELD_LAYER_INDEX, SHIELD_X_OFFSET,
                                 SHIELD_Y_OFFSET);

        // Create starfield layers.
        generate_starfield(&screen, STARFIELD_LAYER_INDEX, 1,
                           SCREEN_TILE_SIZE, SCREEN_TILE_SIZE,
                           NUM_STARFIELD_TILES, STARFIELD_DENSITY / 2, 32, 128);
        generate_starfield(&screen, STARFIELD2_LAYER_INDEX, 2,
                           SCREEN_TILE_SIZE, SCREEN_TILE_SIZE,
                           NUM_STARFIELD_TILES, STARFIELD_DENSITY, 16, 64);
        starfield_y_offset = 0;
        screen.scroll_tile_layer(STARFIELD_LAYER_INDEX, 0, 0);
        screen.scroll_tile_layer(STARFIELD2_LAYER_INDEX, 0, 0);

        // create explosions and player shots
        for (int i = 0; i < num_explosions; ++i) {
            explosions[i].Explosion_init(i, 0, 0, false);
        }
        for (int i = 0; i < num_player_shots; ++i) {
            player_shots[i].Shot_init(i, 0, 0, false);
        }

        player_shot_delay = 225;
        bonus_launch_delay = base_launch_delay;
        int array_select;
        // increase difficulty and chance for bonus points as waves progress
        if (wave < 6) {
            alien_shot_delay = 400 - wave * 25;
            num_alien_shots = 16 + wave;

            current_alien_speed +=
                INT_TO_FIXED(ALIEN_BASE_SPEED) +
                wave * INT_TO_FIXED(ALIEN_WAVE_SPEED_INCREASE);

            array_select = wave;
        } else {  // if (wave >= 6)
            if (alien_shot_delay > 200) {
                alien_shot_delay -= 20;
                num_alien_shots += 3;
                if (num_alien_shots > MAX_NUM_ALIEN_SHOTS)
                    num_alien_shots = MAX_NUM_ALIEN_SHOTS;
            }
            current_alien_speed += INT_TO_FIXED(ALIEN_HIGH_WAVE_SPEED_INCREASE);
            array_select = 6;
        }
        const uint8_t kAlienOddRangeValues[] = {10, 9, 8, 8, 7, 6, 6};
        const uint8_t kBonusSelectMax[] = {5, 5, 4, 4, 3, 2, 2};
        const uint8_t kLaunchDelayMax[] = {4, 4, 3, 3, 3, 3, 2};
        alien_odd_range = kAlienOddRangeValues[array_select];
        for (int i = 0; i < random_list_len; ++i) {
            direction[i] = (rand() % 3);
            bonus_select[i] = (rand() % (kBonusSelectMax[array_select] + 1));
            launch_delay[i] = (rand() % (kLaunchDelayMax[array_select] + 1));
        }
        init_aliens(alien_odd_range);
#ifdef FRAME_COUNTER
        printf_P("wave %d\n", wave + 1);
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
                printf_P("FPS: %d\n", fps);
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
            current_player_speed = 0;
            if (keys.left && !keys.right) {
                current_player_speed = -player_speed;
            }
            if (keys.right && !keys.left) {
                current_player_speed = player_speed;
            }

#ifdef EVENT_COUNTER
        event_counter.start_game_logic_section(0);
#endif
            // alien behavior
            alien_fire();
            launch_bonus_ship();
#ifdef EVENT_COUNTER
        event_counter.end_game_logic_section(0);

        event_counter.start_game_logic_section(1);
#endif
            // move everything
            if (player->is_active())
                player->movement(delta, current_player_speed);
            if (bonus->is_active()) {
                //sound.play_bonus();
                bonus->movement(delta, current_bonus_speed);
            } else {
                //sound.halt_bonus();
            }
            // Cast |delta| to a signed value to correctly multiply.  Otherwise
            // the result is incorrect due to mixing signed and unsigned ints
            // with different widths.
            fixed alien_movement =
                (current_alien_speed * (int32_t)delta) / 1000;
            // All the aliens move in tandem so just update the reference alien.
            reference_alien->movement(delta, alien_movement);
            // Now find an alien on each edge of the formation and use it for
            // edge collision detection.
            int8_t left_col, right_col;
            for (left_col = 0; left_col < ALIEN_ARRAY_WIDTH; ++left_col) {
              if (num_aliens_per_col[left_col] > 0)
                break;
            }
            for (right_col = ALIEN_ARRAY_WIDTH - 1; right_col >= 0;
                 --right_col) {
              if (num_aliens_per_col[right_col] > 0)
                break;
            }
            // Move these edge aliens and test edge collision.
            uint8_t num_aliens_to_test = (left_col == right_col) ? 1 : 2;
            uint8_t alien_cols[] = { left_col, right_col };
            for (uint8_t index = 0; index < num_aliens_to_test; ++index) {
                uint8_t offset;
                for (uint8_t row = 0, offset = 0;
                     row < ALIEN_ARRAY_HEIGHT;
                     ++row, offset += ALIEN_ARRAY_WIDTH) {
                  if (aliens[offset + alien_cols[index]].is_alive()) {
                    Alien alien;
                    make_alien(aliens[offset + alien_cols[index]],
                               reference_alien, &alien);
                    alien.movement(delta, alien_movement);
                    break;
                  }
                }
            }
#ifdef EVENT_COUNTER
        event_counter.end_game_logic_section(1);
        event_counter.start_game_logic_section(2);
#endif
            for (int i = 0; i < num_player_shots; ++i) {
                if (player_shots[i].is_active()) {
                    player_shots[i].movement(delta, shot_speed);
                }
            }
            for (int i = 0; i < num_alien_shots; ++i) {
                if (alien_shots[i].is_active()) {
                    alien_shots[i].movement(delta, alien_shot_speed);
                }
            }
            // explosion duration
            for (int i = 0; i < num_explosions; ++i) {
                if (explosions[i].is_active()) {
                    explosions[i].duration(delta, 0);
                }
            }

            // Move starfields.
            starfield_y_offset += INT_TO_FIXED(delta * STARFIELD_SPEED) / 1000;
#ifdef EVENT_COUNTER
        event_counter.end_game_logic_section(2);
        event_counter.start_game_logic_section(3);
#endif

            // collision handling

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
                uint8_t group;
                if (!collides_with_shield_group(shot, &group))
                    continue;

                for (int i = group * NUM_SHIELDS_PER_GROUP;
                     i < (group + 1) * NUM_SHIELDS_PER_GROUP && i < NUM_SHIELDS;
                     ++i) {
                    //ShieldPiece* shield = &shields[i];
                    if (!shields[i].intact)
                      continue;
                    GameEntity shield;
                    make_shield(shields[i], &shield);
                    if (shield.is_alive() && shield.collides_with(shot)) {
                        shot->shot_shield_collision(&shield);
                        shields[i].intact = false;
                        shield_group_tiles[shields[i].group].
                                update_shield_piece(shields[i]);
                        if (!shot->is_active())
                            break;
                    }
                }
            }
#ifdef EVENT_COUNTER
        event_counter.end_game_logic_section(3);
        event_counter.start_game_logic_section(4);
#endif
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
                              break;
                      }
                  }
            }
#ifdef EVENT_COUNTER
        event_counter.end_game_logic_section(4);
        event_counter.start_game_logic_section(5);
#endif
            // player shots with aliens, bonus, and shields
            for (int j = 0; j < num_player_shots; ++j) {
                Shot* shot = &player_shots[j];
                if (!shot->is_active())
                    continue;
                if (bonus->is_active() && shot->collides_with(bonus)) {
                    bonus->bonus_shot_collision(shot);
                    if (!shot->is_active())
                        break;
                }
                if (!shot->is_active())
                    continue;
                // TODO: Should be able to check only nearby aliens using array.
                for (int i = 0; i < NUM_ALIENS; ++i) {
                    if (!aliens[i].is_alive())
                      continue;
                    // Construct full alien from reduced alien for collision
                    // testing.
                    Alien temp_alien;
                    make_alien(aliens[i], reference_alien, &temp_alien);
                    if (shot->collides_with(&temp_alien)) {
                        shot->shot_alien_collision(&temp_alien);
                        // Update the reduced alien.
                        aliens[i].active = temp_alien.is_active();
                        aliens[i].dirty = temp_alien.is_dirty();
                        if (!temp_alien.is_alive()) {
                          aliens[i].alive = false;
                          --num_aliens_per_col[aliens[i].col];
                        }
                        if (!shot->is_active())
                            break;
                    }
                }
                if (!shot->is_active())
                    continue;
                uint8_t group;
                if (!collides_with_shield_group(shot, &group))
                    continue;
                for (int i = group * NUM_SHIELDS_PER_GROUP;
                     i < (group + 1) * NUM_SHIELDS_PER_GROUP && i < NUM_SHIELDS;
                     ++i) {
                    if (!shields[i].intact)
                      continue;
                    GameEntity shield;
                    make_shield(shields[i], &shield);
                    if (shield.is_alive() && shield.collides_with(shot)) {
                        shot->shot_shield_collision(&shield);
                        shields[i].intact = false;
                        shield_group_tiles[shields[i].group].
                                update_shield_piece(shields[i]);
                        if (!shot->is_active())
                            break;
                    }
                }
            }
#ifdef EVENT_COUNTER
        event_counter.end_game_logic_section(5);
        event_counter.start_game_logic_section(6);
#endif
            // Aliens with shields.
            for (int k = 0; k < ALIEN_ARRAY_HEIGHT; ++k) {
                uint8_t alien_index = k * ALIEN_ARRAY_WIDTH;
                for (int j = 0; j < ALIEN_ARRAY_WIDTH; ++j, ++alien_index) {
                    if (!aliens[alien_index].is_alive())
                        continue;

                    Alien temp_alien;
                    make_alien(aliens[alien_index], reference_alien,
                               &temp_alien);
                    Alien* alien = &temp_alien;

                    // If the alien is higher than the shield groups, so are all
                    // other aliens in the row, so skip the rest of the row.
                    if (alien->get_y() + alien->get_h() < SHIELD_Y_OFFSET)
                        break;

                    uint8_t group;
                    if (!collides_with_shield_group(alien, &group))
                        continue;

                    // Compute the alien collision edges, relative to the upper
                    // left corner of the shield group.
                    int alien_left = alien->get_x() + alien->coll_x_offset() -
                            shield_groups[group].get_x();
                    int alien_right = alien_left + alien->coll_w() - 1;
                    int alien_top = alien->get_y() + alien->coll_y_offset() -
                            shield_groups[group].get_y();
                    int alien_bottom = alien_top + alien->coll_h() - 1;

                    // Now compute the range of shield pieces within the group
                    // that are touching the alien.
                    int shield_left = max(alien_left / SHIELD_PIECE_SIZE, 0);
                    int shield_right = min(alien_right / SHIELD_PIECE_SIZE,
                                           SHIELD_GROUP_WIDTH - 1);
                    int shield_top = max(alien_top / SHIELD_PIECE_SIZE, 0);
                    int shield_bottom = min(alien_bottom / SHIELD_PIECE_SIZE,
                                            SHIELD_GROUP_HEIGHT - 1);

                    uint8_t offset = group * NUM_SHIELDS_PER_GROUP +
                                     shield_top * SHIELD_GROUP_WIDTH;
                    for (int y = shield_top; y <= shield_bottom;
                         ++y, offset += SHIELD_GROUP_WIDTH) {
                        for (int x = shield_left; x <= shield_right; ++x) {
                            if (!shields[offset + x].intact)
                                continue;
                            // Break all pieces that are still intact.  The
                            // alien survives.
                            shields[offset + x].intact = false;
                            shield_group_tiles[group].
                                    update_shield_piece(shields[offset + x]);
                        }
                    }
                }
            }
            // Aliens with player.
            for (int k = 0; k < ALIEN_ARRAY_HEIGHT && player->is_active(); ++k) {
                uint8_t alien_index = k * ALIEN_ARRAY_WIDTH;
                for (int j = 0; j < ALIEN_ARRAY_WIDTH; ++j, ++alien_index) {
                    ReducedAlien& reduced_alien = aliens[alien_index];
                    if (!reduced_alien.is_alive())
                        continue;
                    Alien alien;
                    make_alien(reduced_alien, reference_alien, &alien);
                    // If the alien doesn't overlap with the player along the
                    // vertical axis, skip the entire row of aliens.
                    if (alien.get_y() + alien.get_h() < player->get_y() ||
                        alien.get_y() > player->get_y() + player->get_w()) {
                        break;
                    }
                    if (player->collides_with(&alien)) {
                        player->player_alien_collision(&alien);
                        if (!player->is_alive())
                            break;
                        if (!alien.is_alive()) {
                          reduced_alien.alive = false;
                          --num_aliens_per_col[reduced_alien.col];
                        }
                        reduced_alien.dirty = alien.is_dirty();
                    }
                }
            }
#ifdef EVENT_COUNTER
        event_counter.end_game_logic_section(6);
#endif

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
                current_alien_speed = -current_alien_speed;
                reference_alien->do_alien_logic();
                logic_this_loop = false;
            }

            // draw everything
            screen.begin_update();
#ifdef EVENT_COUNTER
            event_counter.start_game_logic_section(7);
#endif
            if (player->is_dirty())
                player->draw();
            if (bonus->is_dirty())
                bonus->draw();

            for (int i = 0; i < NUM_ALIENS; ++i) {
                if (!aliens[i].is_dirty())
                  continue;
                Alien alien;
                make_alien(aliens[i], reference_alien, &alien);
                alien.draw();
            }
            for (int i = 0; i < num_player_shots; ++i) {
                if (player_shots[i].is_dirty()) {
                    player_shots[i].draw();
                }
            }
            for (int i = 0; i < num_alien_shots; ++i) {
                if (alien_shots[i].is_dirty()) {
                    alien_shots[i].draw();
                }
            }
            for (int i = 0; i < num_explosions; ++i) {
                if (explosions[i].is_dirty()) {
                    explosions[i].draw();
                }
            }
            for (int i = 0; i < NUM_SHIELD_GROUPS; ++i) {
                uint8_t x = i * SHIELD_GROUP_X_SPACING / SCREEN_TILE_SIZE;
                shield_group_tiles[i].draw(&screen, SHIELD_LAYER_INDEX, x,
                                           0);
            }
            // Scroll starfields.  One scrolls slower than the other, for a neat
            // parallax effect.
            screen.scroll_tile_layer(STARFIELD_LAYER_INDEX, 0,
                                     FIXED_TO_INT(starfield_y_offset));
            screen.scroll_tile_layer(STARFIELD2_LAYER_INDEX, 0,
                                     FIXED_TO_INT(starfield_y_offset) * 3 / 4);

#ifdef EVENT_COUNTER
            event_counter.end_game_logic_section(7);
#endif
            screen.update();

#ifdef EVENT_COUNTER
            event_counter.new_loop();
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

        screen.begin_update();
        player->draw();
        bonus->draw();
        for (int i = 0; i < NUM_SHIELD_GROUPS; ++i) {
            uint8_t x = i * SHIELD_GROUP_X_SPACING / SCREEN_TILE_SIZE;
            shield_group_tiles[i].draw(&screen, SHIELD_LAYER_INDEX, x, 0);
        }
        for (int i = 0; i < NUM_ALIENS; ++i) {
            Alien alien;
            make_alien(aliens[i], reference_alien, &alien);
            alien.draw();
        }
        screen.update();
        //sound.play_player_rebirth();
    }
    void Game::wave_cleanup()
    {
        screen.begin_update();
        // redraw all that should remain
        if (player->is_active())
            player->draw();
        for (int i = 0; i < NUM_SHIELD_GROUPS; ++i) {
            uint8_t x = i * SHIELD_GROUP_X_SPACING / SCREEN_TILE_SIZE;
            shield_group_tiles[i].draw(&screen, SHIELD_LAYER_INDEX, x, 0);
        }
        screen.update();
    }
    void Game::pause()
    {
        uint32_t begin_pause;
        //sound.halt_bonus();
        //sound.halt_bg(alien_count);
        begin_pause = System::get_ticks();

        // Wait for the pause key to be released before allowing resume.
        // This way, holding down the pause key will still pause the game.
        while (System::get_key_state().pause)
            System::delay(PAUSE_COOLDOWN_TIME);

        while (!System::get_key_state().pause);

        System::delay(PAUSE_COOLDOWN_TIME);
        last_loop_time +=  System::get_ticks() - begin_pause;
        last_bonus_launch = last_alien_shot = System::get_ticks();
        //sound.play_bonus();
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
            bonus->init_y(BONUS_TOP);
            current_bonus_speed = bonus_speed;
            bonus->activate();
        } else {
            bonus->init_x(screen_w);
            bonus->init_y(BONUS_TOP);
            current_bonus_speed = -bonus_speed;
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
        if (System::get_ticks() - last_alien_shot < alien_shot_delay) {
            return;
        }
        // record time and fire
        last_alien_shot = System::get_ticks();
        static int alien_to_fire = 0;
        ++alien_to_fire;
        for (int i = 0; i < NUM_ALIENS; ++i) {
            ReducedAlien& alien = aliens[i];
            if (alien.is_active() && alien.get_fire_chance() == alien_to_fire) {
                Alien temp_alien;
                make_alien(alien, reference_alien, &temp_alien);
                alien_shots[alien_shot_counter].init_x(temp_alien.get_x()+alien_init_x_shot_pos);
                alien_shots[alien_shot_counter].init_y(temp_alien.get_y()+alien_init_y_shot_pos);
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
    void Game::msg_alien_killed(int index, int points)
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
        // Set the alien above the one just killed to active.  If the above one
        // is dead, try the one above that.
        int next_alien_index = index - ALIEN_ARRAY_WIDTH;
        while (next_alien_index >= 0) {
            if (aliens[next_alien_index].is_alive()) {
                aliens[next_alien_index].activate();
                break;
            }
            next_alien_index -= ALIEN_ARRAY_WIDTH;
        }

        // speed up all the existing aliens, whenever one is destroyed
        current_alien_speed =
           increase_speed(current_alien_speed, ALIEN_SPEED_BOOST);
        switch (alien_count) {
        case 4:
        case 2:
        case 1:
            current_alien_speed =
                increase_speed(current_alien_speed,
                               ALIEN_SPEED_BOOST_EXTRA);
            //sound.halt_bg(alien_count);
            break;
        case 3:
            //sound.halt_bg(alien_count);
            break;
        default:
            break;
        }
    }
    bool Game::collides_with_shield_group(GameEntity* object, uint8_t* group) {
        for (int i = 0; i < NUM_SHIELD_GROUPS; ++i) {
            GameEntity* shield_group = &shield_groups[i];
            if (shield_group->collides_with(object)) {
                *group = i;
                return true;
            }
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
    Game::Game(Screen* screen_ptr) :
                   // ui(&sound, this, 0),
                   screen(*screen_ptr),
                   player_life(0),
                   player(NULL),
                   bonus(NULL),
                   sbonus(NULL),
                   rbonus(NULL)
    {
        GameEntity::set_game(this);
        GameEntity::set_screen(&screen);
    }
    Game::~Game()
    {
        GameEntity::set_game(NULL);
    }
}
