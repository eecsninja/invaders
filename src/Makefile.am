bin_PROGRAMS = classic_invaders
classic_invaders_SOURCES = alien.cpp alien.h bonus_ship.cpp bonus_ship.h \
explosion.h game.cpp game.h game_entity.cpp game_entity.h main.cpp \
player.cpp player.h rand_num_gen.h screen.h shield_piece.h shot.cpp shot.h \
sound.cpp sound.h status.cpp status.h ttf.cpp ttf.h ui.cpp ui.h
if CYGWIN
classic_invaders_CPPFLAGS = -I/usr/local/include -DDATADIR=\"$(datadir)\"
else
classic_invaders_CPPFLAGS = -DDATADIR=\"$(datadir)\"
endif
