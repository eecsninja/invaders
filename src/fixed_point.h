/*
 fixedpoint.h
 Classic Invaders

 Copyright (c) 2013, Todd Steinackle, Simon Que
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

#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>

typedef int16_t fixed;    // Used for fixed point math on embedded systems.
#define FIXED_POINT_FACTOR                              16
#define FIXED_POINT_SHIFT                                4
// Convert to 32-bit int for multiplication, to avoid losing precision.
#define INT_TO_FIXED(x)           (((int32_t)x) << FIXED_POINT_SHIFT)
#define FIXED_TO_INT(x)           (x >> FIXED_POINT_SHIFT)

typedef int32_t fixed32;    // 32-bit fixed-point value for greater precision.
#define FIXED_POINT_FACTOR_32               (1 << FIXED_POINT_SHIFT_32)
#define FIXED_POINT_SHIFT_32                             8
#define INT_TO_FIXED32(x)           (((int32_t)x) << FIXED_POINT_SHIFT_32)
#define FIXED32_TO_INT(x)           (x >> FIXED_POINT_SHIFT_32)


#endif  // FIXED_POINT_H
