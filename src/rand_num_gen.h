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

#include <ctime>

typedef boost::mt19937 base_generator_type;
typedef boost::variate_generator<base_generator_type&, boost::uniform_int<> > generator_fun;

// boost random number generator functions
base_generator_type generator(static_cast<unsigned int>(std::time(0)));
boost::uniform_int<> two(1,2);
boost::uniform_int<> three(1,3);
boost::uniform_int<> four(1,4);
boost::uniform_int<> five(1,5);
boost::uniform_int<> six(1,6);
boost::uniform_int<> seven(1,7);
boost::uniform_int<> eight(1,8);
boost::uniform_int<> nine(1,9);
boost::uniform_int<> ten(1,10);
generator_fun gen1_2(generator, two);
generator_fun gen1_3(generator, three);
generator_fun gen1_4(generator, four);
generator_fun gen1_5(generator, five);
generator_fun gen1_6(generator, six);
generator_fun gen1_7(generator, seven);
generator_fun gen1_8(generator, eight);
generator_fun gen1_9(generator, nine);
generator_fun gen1_10(generator, ten);

#endif  //RAN_NUM_GEN_H
