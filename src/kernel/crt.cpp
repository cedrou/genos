//------------------------------------------------------------------------------
// crt.cpp
//	Microsoft Visual C++ runtime (msvcrt) compatibility
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

#include "crt.h"

using namespace GenOS;


// Initializers
//-----------------------
#pragma data_seg(".CRT$XCA")
initmethod Crt::__xc_a[] = { NULL }; // Beginning of initializer table

#pragma data_seg(".CRT$XCZ")
initmethod Crt::__xc_z[] = { NULL }; // End of initializer table

#pragma data_seg()  // Go back to default data segment (.data)

#pragma comment(linker, "/merge:.CRT=.data")// Merge .CRT into .data section


// Finalizers
//-----------------------
initmethod* Crt::finalizers = NULL;
uint32      Crt::finalizers_max;
uint32      Crt::finalizers_count;


void Crt::Start(uint32 crtStart, uint32 crtSize) 
{
  finalizers = (initmethod*)crtStart;
  finalizers_max = crtSize / sizeof(initmethod);
  finalizers_count = 0;

  uint8 i = 0; 
  for( initmethod* current = __xc_a; current < __xc_z; current++ )
  {
    ((uint16*)0xB8000)[0] = (0x0F << 8) | ('A' + i++);

    if ( *current )
      (**current)();
  }
}

void Crt::Shutdown() 
{
  while (finalizers_count--) {
    (*(--finalizers)) ();
  }
}

int Crt::atexit(initmethod fn) 
{
  if ( finalizers_count < finalizers_max )
  {
    // Add the exit routine
    *(finalizers++) = fn;
    finalizers_count++;
    return 0;
  }

  return 1;
}

extern "C" int atexit(initmethod fn) 
{
  return Crt::atexit(fn);
}


