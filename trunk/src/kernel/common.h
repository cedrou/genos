//------------------------------------------------------------------------------
// common.h
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

#pragma once

// These typedefs are written for 32-bit X86.
typedef unsigned char     uint8;
typedef          char     int8;
typedef unsigned short    uint16;
typedef          short    int16;
typedef unsigned int      uint32;
typedef          int      int32;
typedef unsigned __int64  uint64;
typedef          __int64  int64;
typedef unsigned int      size_t;
typedef          void*    intptr;

typedef          void* paddr;
typedef          void* vaddr;

intptr memcpy(intptr dst, const intptr src, size_t count);

void memset(intptr dst, uint8 value, size_t count);
void memset(intptr dst, uint16 value, size_t count);
void memset(intptr dst, uint32 value, size_t count);

int32 memcmp(const intptr a, const intptr b, size_t size);

#ifndef WIN32

// construct array with placement at _Where
inline void* __cdecl operator new(size_t, void* placement) { return placement; }
// delete if placement new fails
inline void __cdecl operator delete(void*, void*) { }

// construct array with placement at _Where
inline void* __cdecl operator new[](size_t, void* placement) { return placement; }
// delete if placement new fails
inline void __cdecl operator delete[](void*, void*) { }

#include "registers.h"
#include "kernel.h"

#else

#include <assert.h>
#define ASSERT(x) assert(x)

#endif

#define NULL 0
