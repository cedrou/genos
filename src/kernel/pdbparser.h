//------------------------------------------------------------------------------
// pdbparser.h
//	Debug info
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

#pragma once

#include "common.h"
#include "sortedarray.h"

namespace GenOS
{
  class PdbParser
  {
  private:
    struct Header
    {
      uint32 PageSize;
      uint32 BitmapPage;
      uint32 MaxPages;
      uint32 RootSize;
      uint32 Reserved;
      uint32 RootPage;
    };

  public:
    struct Record
    {
      uint16 Size;
      uint16 Type;
    };

    // Public Symbol - type=110E
    struct PublicSymbolEntry : public Record
    {
      uint32  Flags;
      uint32  Offset;
      uint16  Segment;
      uint8   Name[1];
    };

  private:
    const uint8*  _pdb;
    const size_t  _size;
    const Header* _header;

    const uint32* _root;
    uint32        _nbStreams;
    const uint32* _streamSizes;
    const uint32* _streamPages;

    uint8**       _streamsCache;
    SortedArray<PublicSymbolEntry*> _symbols;

  public:
    PdbParser(const uint8* pdb, const size_t& size);

          uint32  PdbByteSize() const;
          uint32  PdbPageSize() const;

    const uint8*  Stream(uint32 sid) const;
          uint32  StreamByteSize(uint32 sid) const;
          uint32  StreamPageSize(uint32 sid) const;
    const uint32* StreamPages(uint32 sid) const;
          uint32  NbStreams() const;

          void    ParseTypes();
          void    ParseDebugInfo();

    const SortedArray<PublicSymbolEntry*>& Symbols() const;

    const PublicSymbolEntry* GetSymbol(uint16 seg, uint32 off) const;

  private:
    void Initialize();
    static int32 CompareEntries(PublicSymbolEntry* const & a, PublicSymbolEntry* const & b);

          uint32  PageToOffset(uint32 page) const;
          uint32  BytesToPages(uint32 bytes) const;

  };
}