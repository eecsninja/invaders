/*
 main.cpp
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

#include <stdio.h>
#include <stdlib.h>

#include <avr/pgmspace.h>

#include <DuinoCube.h>
#include <SPI.h>

#if defined(__AVR_ATmega32U4__)
#include <Esplora.h>
#endif  // defined(__AVR_ATmega32U4__)

#include "game.h"
#include "game_defs.h"
#include "printf.h"
#include "screen.h"
#include "system.h"

extern uint8_t __bss_end;
extern uint8_t __stack;

// VRAM offsets of image data.
uint16_t g_vram_offsets[NUM_GAME_ENTITY_TYPES];

namespace {

const char kFilePath[] = "invaders";

struct File {
  const char* filename;
  uint16_t* vram_offset;  // For image data, compute and store VRAM offset here.
  uint16_t addr;          // For other data, store data here.
  uint16_t bank;          // Bank to use for |addr|.
  uint16_t max_size;      // Size checking to avoid overflow.
};

// Filenames.  To avoid cumbersome declarations of multiple strings, these are
// combined into one long string, delimited by null terminators.
const char kFilenames[] PROGMEM =
  "ship.raw\0"
  "alien1.raw\0"
  "alien2.raw\0"
  "alien3.raw\0"
  "bonus.raw\0"
  "bonus_sm.raw\0"
  "shot.raw\0"
  "shield.raw\0"
  "explode.raw\0"
  "palette.pal\0"
;

// Image, palette, and tilemap data.
const File kFiles[] PROGMEM = {
  // Image data.
  { NULL, &g_vram_offsets[GAME_ENTITY_PLAYER], 0, 0, VRAM_BANK_SIZE },
  { NULL, &g_vram_offsets[GAME_ENTITY_ALIEN], 0, 0, VRAM_BANK_SIZE },
  { NULL, &g_vram_offsets[GAME_ENTITY_ALIEN2], 0, 0, VRAM_BANK_SIZE },
  { NULL, &g_vram_offsets[GAME_ENTITY_ALIEN3], 0, 0, VRAM_BANK_SIZE },
  { NULL, &g_vram_offsets[GAME_ENTITY_BONUS_SHIP], 0, 0, VRAM_BANK_SIZE },
  { NULL, &g_vram_offsets[GAME_ENTITY_SMALL_BONUS_SHIP], 0, 0, VRAM_BANK_SIZE },
  { NULL, &g_vram_offsets[GAME_ENTITY_SHOT], 0, 0, VRAM_BANK_SIZE },
  { NULL, &g_vram_offsets[GAME_ENTITY_SHIELD_PIECE], 0, 0, VRAM_BANK_SIZE },
  { NULL, &g_vram_offsets[GAME_ENTITY_EXPLOSION], 0, 0, VRAM_BANK_SIZE },

  // Palette data.
  { NULL, NULL, PALETTE(BASE_PALETTE_INDEX), 0, PALETTE_SIZE },
};

// Load image, palette, and tilemap data from file system.
void loadResources() {
  uint16_t string_offset = 0;
  uint16_t vram_offset = 0;
  for (int i = 0; i < sizeof(kFiles) / sizeof(kFiles[0]); ++i) {
    // Read file info from program memory.
    File file;
    memcpy_P(&file, kFiles + i, sizeof(file));

    char filename[256];
    sprintf(filename, "%s/", kFilePath);
    size_t len = strlen(filename);
    // Append the filename from program memory.
    size_t filename_len = strlen_P(kFilenames + string_offset);
    if (filename_len + len + 1 > sizeof(filename)) {
      printf_P("Filename is too long.\n");
      continue;
    }
    memcpy_P(filename + len, kFilenames + string_offset, filename_len + 1);
    string_offset += filename_len + 1;    // Update offset to next filename.

    // Open the file.
    uint16_t handle = DC.File.open(filename, FILE_READ_ONLY);
    if (!handle) {
      printf_P("Could not open file %s.\n", filename);
      continue;
    }

    uint16_t file_size = DC.File.size(handle);
    printf_P("File %s is 0x%x bytes\n", filename, file_size);

    if (file_size > file.max_size) {
      printf_P("File is too big!\n");
      DC.File.close(handle);
      continue;
    }

    // Compute write destination.
    uint16_t dest_addr;
    uint16_t dest_bank;

    if (file.vram_offset) {
      // Set up for VRAM write.

      // If this doesn't fit in the remaining part of the current bank, use the
      // next VRAM bank.
      if (vram_offset % VRAM_BANK_SIZE + file_size > VRAM_BANK_SIZE)
        vram_offset += VRAM_BANK_SIZE - (vram_offset % VRAM_BANK_SIZE);

      // Record VRAM offset.
      *file.vram_offset = vram_offset;

      // Determine the destination VRAM address and bank.
      dest_addr = VRAM_BASE + vram_offset % VRAM_BANK_SIZE;
      dest_bank = vram_offset / VRAM_BANK_SIZE + VRAM_BANK_BEGIN;
      DC.Core.writeWord(REG_SYS_CTRL, (1 << REG_SYS_CTRL_VRAM_ACCESS));

      // Update the VRAM offset.
      vram_offset += file_size;
    } else {
      // Set up for non-VRAM write.
      dest_addr = file.addr;
      dest_bank = file.bank;
    }

    printf_P("Writing to 0x%x with bank = %d\n", dest_addr, dest_bank);
    DC.Core.writeWord(REG_MEM_BANK, dest_bank);
    DC.File.readToCore(handle, dest_addr, file_size);

    DC.File.close(handle);
  }

  // Set to bank 0.
  DC.Core.writeWord(REG_MEM_BANK, 0);

  // Allow the graphics pipeline access to VRAM.
  DC.Core.writeWord(REG_SYS_CTRL, (0 << REG_SYS_CTRL_VRAM_ACCESS));
}

}  // namespace

void setup() {
    Serial.begin(115200);
    DC.begin();

    printf_P("Stack ranges from 0x%x (%u) to 0x%x (%u)\n",
             &__bss_end, &__bss_end, &__stack, &__stack);

    Graphics::Screen screen;
    printf_P("Allocated screen controller: %u bytes at 0x%x (%u bytes)\n",
             sizeof(screen), &screen, &screen);

    loadResources();

    // Initialize video screen and image library.
    screen.init();

    Game::Game game(&screen);
    game.game_control();

    printf_P("End of program.\n");
}


void loop () {
}
