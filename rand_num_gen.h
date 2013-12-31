/*
 rand_num_gen.h
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

#ifndef RAN_NUM_GEN_H
#define RAN_NUM_GEN_H

#include <stdlib.h>

#define RAND(max) (rand() % (max + 1))

#define CREATE_RAND_GENERATOR(name, max) static int name() { return RAND(max); }

CREATE_RAND_GENERATOR(gen1_2, 2);
CREATE_RAND_GENERATOR(gen1_3, 3);
CREATE_RAND_GENERATOR(gen1_4, 4);
CREATE_RAND_GENERATOR(gen1_5, 5);
CREATE_RAND_GENERATOR(gen1_6, 6);
CREATE_RAND_GENERATOR(gen1_7, 7);
CREATE_RAND_GENERATOR(gen1_8, 8);
CREATE_RAND_GENERATOR(gen1_9, 9);
CREATE_RAND_GENERATOR(gen1_10, 10);

#endif  //RAN_NUM_GEN_H
