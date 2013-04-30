#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([Classic Invaders], [1.0], [todd@noquarterarcade.com], [classic-invaders], [http://www.noquarterarcade.com/])
AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/alien.cpp])
AC_CANONICAL_HOST
AC_DEFINE(AUTOTOOLS_INSTALL)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Check for cygwin
AM_CONDITIONAL([CYGWIN], [test $host = "i686-pc-cygwin"])

# Checks for libraries.
SDL_VERSION=1.2.11
AC_SUBST(SDL_VERSION)
AM_PATH_SDL($SDL_VERSION,
            :,
	    AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CHECK_LIB([SDL_image], [IMG_Load], , AC_MSG_ERROR([
*** Unable to find SDL_image libary
(http://www.libsdl.org/projects/SDL_image/)
]))

AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio], , AC_MSG_ERROR([
*** Unable to find SDL_mixer libary
(http://www.libsdl.org/projects/SDL_mixer/)
]))

AC_CHECK_LIB([SDL_ttf], [TTF_Init], , AC_MSG_ERROR([
*** Unable to find SDL_ttf libary
(http://www.libsdl.org/projects/SDL_ttf/)
]))

BOOST_REQUIRE
BOOST_SMART_PTR
BOOST_ASSIGN_LIST_OF
BOOST_RANDOM
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

echo \
"=============================================================================

${PACKAGE_NAME} -- Version ${PACKAGE_VERSION}
prefix: ${prefix}
datadir: ${datadir}
datarootdir: ${datarootdir}

Note: the data directory is passed in at compile time so if you configure
again with different prefix, datadir, or datarootdir options you will have to
run -- make clean

The No Quarter Arcade
http://www.noquarterarcade.com/

=============================================================================="
