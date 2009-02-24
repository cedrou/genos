//------------------------------------------------------------------------------
// bitmanip.h
//	Implements several optimized bit twiddling algorithms
//
// See http://graphics.stanford.edu/~seander/bithacks.html
//------------------------------------------------------------------------------
// Copyright (c) 2008, Cedric Rousseau
// All rights reserved.
// 
// This source code is released under the new BSD License.
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions, and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions, and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include "common.h"

namespace GenOS {

  class BitManip
  {
  private:
    static const char LogTable256[];
    static const uint8 BitsSetTable256[]; 

  public:
    inline static void SetBit(uint32& value, uint32 bit)
    {
      value |= (1 << bit);
    }

    inline static void ClearBit(uint32& value, uint32 bit)
    {
      value &= ~(1 << bit);
    }

    // Integer log base 2 of an integer
    // Find the position of the highest bit set
    inline static uint32 HighestBitSet(uint32 value)
    {
      uint32 bit = 0;
      if (uint32 a = value >> 16)
      {
        uint32 b = a >> 8;
        bit = b ? 24 + LogTable256[b] : 16 + LogTable256[a];
      }
      else 
      {
        uint32 b = value >> 8;
        bit = b ? 8 + LogTable256[b] : LogTable256[value];
      }

      return bit;
    }

    // Count the trailing zero bits on the right
    // Find the position of the lowest bit set
    inline static uint32 LowestBitSet(uint32 value)
    {
      uint32 count = 0;
      value = (value ^ (value - 1)) >> 1;  // Set value's trailing 0s to 1s and zero rest
      for (count = 0; value; count++)
      {
        value >>= 1;
      }
      return count;
    }

    // Round up to the nearest power of two
    // Find the least power of 2 greater than or equal to an integer
    inline static uint32 RoundUpPow2(uint32 value)
    {
      value = value - 1; 
      value = value | (value >> 1); 
      value = value | (value >> 2); 
      value = value | (value >> 4); 
      value = value | (value >> 8); 
      value = value | (value >>16); 
      return value + 1; 
    } 

    // Round down to the nearest power of two
    // Find the greatest power of 2 less than or equal to an integer
    inline static uint32 RoundDownPow2(uint32 value)
    {
      value = value | (value >> 1); 
      value = value | (value >> 2); 
      value = value | (value >> 4); 
      value = value | (value >> 8); 
      value = value | (value >>16); 
      return value - (value >> 1); 
    }

    inline static uint8 CountBitSet(uint32 value)
    {
      unsigned char* p = (unsigned char*) &value;
      return BitsSetTable256[p[0]] + 
             BitsSetTable256[p[1]] + 
             BitsSetTable256[p[2]] +	
             BitsSetTable256[p[3]];
    }

    inline static uint8 CountBitSet(uint64 value)
    {
      unsigned char* p = (unsigned char*) &value;
      return BitsSetTable256[p[0]] + BitsSetTable256[p[1]] + 
             BitsSetTable256[p[2]] + BitsSetTable256[p[3]] +
             BitsSetTable256[p[4]] + BitsSetTable256[p[5]] +
             BitsSetTable256[p[6]] + BitsSetTable256[p[7]];
    }
  };
}