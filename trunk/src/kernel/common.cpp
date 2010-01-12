//------------------------------------------------------------------------------
// common.cpp
//	Declare some global constants and functions
//------------------------------------------------------------------------------
// This file is part of the GenOS (Genesis Operating System) project.
// The latest version can be found at http://code.google.com/p/genos
//------------------------------------------------------------------------------
// Copyright (c) 2008-2010 Cedric Rousseau
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

#include "common.h"

using namespace GenOS;

intptr memcpy(intptr dst, const intptr src, size_t count)
{
  const char* s = (const char*)src;
  char* d = (char*)dst;
  while(count--) { *d++ = *s++; }
  return dst;
}

#pragma optimize( "", off )

void memset(intptr dst, uint8 value, size_t count)
{
  uint8* d = (uint8*)dst;
  while(count--) { *d++ = value; }
}

#pragma optimize( "", on )

void memset(intptr dst, uint16 value, size_t count)
{
  uint16* d = (uint16*)dst;
  while(count--) { *d++ = value; }
}

void memset(intptr dst, uint32 value, size_t count)
{
  uint32* d = (uint32*)dst;
  while(count--) { *d++ = value; }
}

int memcmp(const intptr a, const intptr b, size_t size)
{
  uint8* ua = (uint8*) a;
  uint8* ub = (uint8*) b;
  while (size--) {
    if (*ua != *ub)
      return (*ua < *ub) ? -1 : +1;
    ua++;
    ub++;
  }
  return 0;
}



int __cdecl ::_purecall() 
{ 
  PANIC("Pure function call");
}

extern "C" int _fltused = 1;



