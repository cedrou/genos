//------------------------------------------------------------------------------
// kheap.cpp
//	Kernel heap
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

#include "kheap.h"
#include "kernel.h"
#include "pagemanager.h"
#include "screen.h"

using namespace GenOS;

#define KHEAP_HEAD_USED 'KHHU'
#define KHEAP_FOOT_USED 'KHFU'
#define KHEAP_HEAD_FREE 'KHHF'
#define KHEAP_FOOT_FREE 'KHFF'

//static Kheap* g_heap = NULL;

static intptr __cdecl _new(uint32 size)
{
  //return g_heap->Allocate(size, 0);
  return Kernel::Kheap()->Allocate(size, 0);
}

static void __cdecl _delete(intptr p)
{
  //return g_heap->Free(p);
  return Kernel::Kheap()->Free(p);
}

static intptr __cdecl panic_new(uint32)
{
  PANIC("Cannot use operator_new because KHeap is not initialized");
  return NULL;
}

static void __cdecl panic_delete(intptr)
{
  PANIC("Cannot use operator_new because KHeap is not initialized");
}


static intptr (__cdecl *bridge_new)(uint32 size) = panic_new;
static void (__cdecl *bridge_delete)(intptr p) = panic_delete;


intptr __cdecl ::operator new(uint32 size)
{
  return bridge_new(size);
}
intptr __cdecl operator new[](uint32 size)
{
  return bridge_new(size);
}
void __cdecl ::operator delete(intptr p)
{
  return bridge_delete(p);
}
void __cdecl operator delete[](intptr p)
{
  return bridge_delete(p);
}




vaddr Kheap::CreateEntryPlacement(vaddr& placement, size_t size, bool free)
{
  const uint32 entrySize = sizeof(EntryHead) + size + sizeof(EntryFoot);

  EntryHead* head = (EntryHead*)placement;
  EntryFoot* foot = (EntryFoot*)((uint32)placement + entrySize - sizeof(EntryHead));

  head->Magic = free ? KHEAP_HEAD_FREE : KHEAP_HEAD_USED;
  foot->Magic = free ? KHEAP_FOOT_FREE : KHEAP_FOOT_USED;
  head->Size = entrySize;
  foot->Size = entrySize;

  placement = (vaddr)((uint32)placement + entrySize);
  return (vaddr)((uint32)head + sizeof(EntryHead));
}

int32 Kheap::CompareEntries(const intptr& a, const intptr& b)
{
  EntryHead* aa = (EntryHead*)a;
  EntryHead* bb = (EntryHead*)b;

  if(aa->Size == bb->Size) return 0;
  if(aa->Size < bb->Size) return -1;
  return 1;
}

//Kheap::Kheap(vaddr heapStart, size_t heapSize, size_t heapMaxSize, SortedArray<intptr>* freeList)
//: _heapStart(heapStart)
//, _heapSize(heapSize)
//, _heapMaxSize(heapMaxSize)
//, _freeList(freeList)
//{
//}
Kheap::Kheap()
{
}

Kheap::~Kheap()
{
}

void Kheap::Initialize(vaddr heapStart, size_t heapMaxSize)
{
  _heapStart = heapStart;
  _heapSize = 0x1000;
  _heapMaxSize = heapMaxSize;

  // Allocate an initial page
  if( !Kernel::PageManager()->Map(0, _heapStart, PageManager::Page::Writable))
  {
    PANIC("Cannot map the first page");
  }

  vaddr placement = _heapStart;

  _freeList = (SortedArray<intptr>*)CreateEntryPlacement(placement, sizeof(SortedArray<intptr>), false);
  intptr* freeListBuffer = (intptr*)CreateEntryPlacement(placement, 4*sizeof(intptr), false);

  _freeList = new (_freeList) SortedArray<intptr>(freeListBuffer, 4, CompareEntries);
  
  intptr freeEntry = placement;

  const size_t initialMemory = 0x1000;
  const size_t freeMemory = initialMemory - 4*(sizeof(EntryHead)+sizeof(EntryFoot)) - sizeof(Kheap) - sizeof(SortedArray<intptr>) - 4*sizeof(intptr);

  CreateEntryPlacement(placement, freeMemory, true);

  _freeList->Insert(freeEntry);


  bridge_new = _new;
  bridge_delete = _delete;
}

intptr Kheap::Allocate(size_t size, uint32 alignment)
{
  // Must be a power of two
  ASSERT( !alignment || (alignment & (alignment-1))==0 );

  //Screen::cout << "initial size: " << size;

  size += sizeof(EntryHead) + sizeof(EntryFoot);
  //Screen::cout << " - allocated size: " << size;


  // Find the first free zone that fits the request
  //Screen::cout << " - freeList size: " << _freeList->Size();
  for(size_t index=0; index<_freeList->Size(); index++)
  {
    EntryHead* head = (EntryHead*)_freeList->At(index);
    //Screen::cout << " - currentHead: " << head;
    if(alignment)
    {
      // TODO
    }
    else if(head->Size >= size)
    {
      //Screen::cout << " - index: " << index;
      _freeList->Remove(index);

      // Check that there is enough room after this entry to create another one.
      // Else, increase the size.
      if (head->Size - size < sizeof(EntryHead) + sizeof(EntryFoot))
      {
        size = head->Size;
        //Screen::cout << " - new size: " << size;
      }
      else
      {
        EntryHead* newFreeHead = (EntryHead*)((uint32)head + size);
        EntryFoot* newFreeFoot = (EntryFoot*)((uint32)head + head->Size - sizeof(EntryFoot));

        newFreeHead->Magic = KHEAP_HEAD_FREE;
        newFreeHead->Size = head->Size - size;
        newFreeFoot->Magic = KHEAP_FOOT_FREE;
        newFreeFoot->Size = head->Size - size;

        _freeList->Insert(newFreeHead);
      }

      EntryHead* newBlockHead = head;
      EntryFoot* newBlockFoot = (EntryFoot*)((uint32)head + size - sizeof(EntryFoot));

      newBlockHead->Magic = KHEAP_HEAD_USED;
      newBlockHead->Size = size;
      newBlockFoot->Magic = KHEAP_FOOT_USED;
      newBlockFoot->Size = size;

      //Screen::cout << Screen::endl;

      return (intptr)((uint32)head + sizeof(EntryHead));
    }
  }

  // Not enough room... Map new page
  // TODO

  return NULL;
}

void Kheap::Free(intptr data)
{
  EntryHead* head = (EntryHead*)((uint32)data - sizeof(EntryHead));
  EntryFoot* foot = (EntryFoot*)((uint32)head + head->Size - sizeof(EntryFoot));

  head->Magic = KHEAP_HEAD_FREE;
  foot->Magic = KHEAP_FOOT_FREE;

  //Screen::cout << "  head " << head << ", foot " << foot << Screen::endl;

  // Merge previous entry
  EntryHead* previousHead = NULL;
  EntryFoot* previousFoot = (EntryFoot*)((uint32)head - sizeof(EntryFoot));
  if(previousFoot->Magic == KHEAP_FOOT_FREE)
  {
    previousHead = (EntryHead*)((uint32)head - previousFoot->Size);
    ASSERT(previousHead->Magic == KHEAP_HEAD_FREE);

    //Screen::cout << "  previousHead " << previousHead << ", previousFoot " << previousFoot << Screen::endl;

    // Save index
    uint32 index = _freeList->Find(previousHead);

    // Change the size in header and footer
    previousHead->Size += head->Size;
    foot->Size = previousHead->Size;

    // Set new header
    head = previousHead;

    // Update the free list
    _freeList->Remove(index);
    _freeList->Insert(previousHead);
  }

  // Merge next entry
  EntryHead* nextHead = (EntryHead*)((uint32)head + head->Size);
  if(nextHead->Magic == KHEAP_HEAD_FREE)
  {
    EntryFoot* nextFoot = (EntryFoot*)((uint32)nextHead + nextHead->Size - sizeof(EntryFoot));
    ASSERT(nextFoot->Magic == KHEAP_FOOT_FREE);

    //Screen::cout << "  nextHead " << nextHead << ", nextFoot " << nextFoot << Screen::endl;

    // Change the size in header and footer
    head->Size += nextHead->Size;
    nextFoot->Size = head->Size;

    // Update the free list
    _freeList->Remove(nextHead);
  }


  if (head != previousHead)
  {
    _freeList->Insert(head);

  }
}

