//------------------------------------------------------------------------------
// framemanager.h
//	Physical memory pages allocator
//------------------------------------------------------------------------------
// Copyright (c) 2008, Cedric Rousseau
// All rights reserved.
// 
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

uint32* FrameManager::_bitset = NULL;
uint32  FrameManager::_bitset_items = 0;

void FrameManager::Initialize(intptr start, uint32 size)
{
  // Place the bitset at the start of the free memory block
  _bitset = (uint32*)start;

  // Compute the total number of frames and the size (in 32 bits words)
  // of the bitset
  uint32 frames = size / 0x1000;
  _bitset_items = frames / 32;

  // Initialize the bitset to 1 (all frames are available)
  memset(_bitset, 0xFFFFFFFF, _bitset_items);

  // Compute the number of frames used by the bitset
  uint32 bitset_size = _bitset_items * 8;
  uint32 bitset_frames = (bitset_size + 0x0FFF) / 0x1000;

  // Clear the corresponding bits in the bitset
  for(uint32 index=0; index<bitset_frames; index++)
  {
    uint32 item = (index >> 5);    // frame_index / 32;
    uint32 bit = (index & 0x1F);   // frame_index % 32
    _bitset[item] &= ~(1 << bit);
  }
}

void FrameManager::Set(intptr frame)
{
  uint32 index = ((uint32)frame >> 12);  // frame / 0x1000;
  uint32 item = (index >> 5);    // frame_index / 32;
  uint32 bit = (index & 0x1F);   // frame_index % 32
  _bitset[item] |= (1 << bit);
}

void FrameManager::Clear(intptr frame)
{
  uint32 index = ((uint32)frame >> 12);  // frame / 0x1000;
  uint32 item = (index >> 5);    // frame_index / 32;
  uint32 bit = (index & 0x1F);   // frame_index % 32
  _bitset[item] &= ~(1 << bit);
}

intptr FrameManager::FindFirst()
{
  for (uint32 item=0; item<_bitset_items; item++)
  {
    uint32 current = _bitset[item];
    if (current != 0)
    {
      uint32 bit = BitManip::Log2(current);
      return (intptr)((bit + (item << 5)) << 12);
    }
  }

  return NULL;
}


// Function to allocate a frame.
intptr FrameManager::Get()
{
  intptr frame = FindFirst();
  Clear(frame);
  return frame;
}

// Function to deallocate a frame.
void FrameManager::Release(intptr frame)
{
  if (frame)
  {
    Set(frame);
  }
} 