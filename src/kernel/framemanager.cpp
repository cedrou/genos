//------------------------------------------------------------------------------
// framemanager.h
//	Physical memory pages allocator
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

#include "framemanager.h"
#include "bitmanip.h"
#include "screen.h"

using namespace GenOS;

paddr   FrameManager::_base = NULL;
uint32* FrameManager::_bitset = NULL;
uint32  FrameManager::_bitset_items = 0;

void FrameManager::Initialize(paddr base, vaddr bitset)
{
  _base = base;

  // Place the size of the bitset at the start of the free memory block
  _bitset_items = ((uint32*)bitset)[0];

  // The bitset immediately follows this value
  _bitset = &((uint32*)bitset)[1];
}

paddr FrameManager::FindFreeFrame()
{
  for (uint32 item=0; item<_bitset_items; item++)
  {
    uint32 current = _bitset[item];
    if (current != 0)
    {
      uint32 bit = BitManip::LowestBitSet(current);
      return (paddr)((uint32)_base + ((bit + (item << 5)) << 12));
    }
  }

  return NULL;
}


// Function to allocate a frame.
paddr FrameManager::GetFrame()
{
  paddr frame = FindFreeFrame();
  
  const uint32 index = (((uint32)frame - (uint32)_base) >> 12);  // frame / 0x1000;
  const uint32 item = (index >> 5);    // frame_index / 32;
  const uint32 bit = (index & 0x1F);   // frame_index % 32

  BitManip::ClearBit(_bitset[item], bit);

  return frame;
}

// Function to deallocate a frame.
void FrameManager::ReleaseFrame(paddr frame)
{
  if (frame == NULL)
    return;
  
  const uint32 index  = (((uint32)frame - (uint32)_base) >> 12); // frame / 0x1000;
  const uint32 item   = (index >> 5);          // frame_index / 32;
  const uint32 bit    = (index & 0x1F);        // frame_index % 32;

  BitManip::SetBit(_bitset[item], bit);
} 