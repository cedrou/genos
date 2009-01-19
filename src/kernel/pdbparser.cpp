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

#include "pdbparser.h"


using namespace GenOS;

#define PDB_SIGNATURE "Microsoft C/C++ MSF 7.00\r\n\x1A" "DS\0\0\0"
#define PDB_SIGNATURE_SIZE (0x20)


PdbParser* PdbParser::Instance = NULL;

PdbParser::PdbParser(const uint8* pdb, const size_t& size)
  : _pdb(pdb), _size(size), _symbols(CompareEntries)
{
  ASSERT( _pdb != 0 );
  ASSERT( _size != 0 );
  ASSERT( memcmp((intptr)_pdb, PDB_SIGNATURE, PDB_SIGNATURE_SIZE)==0 );

  _header = (Header*)(_pdb + PDB_SIGNATURE_SIZE);

  _root = (uint32*)(_pdb + PageToOffset(_header->RootPage-1));
  _nbStreams = *_root;
  _streamSizes = (uint32*)(_root + 1);
  _streamPages = (uint32*)(_streamSizes + _nbStreams);
  
  _streamsCache = new uint8*[_nbStreams];
  memset(_streamsCache, (uint8)0, _nbStreams * sizeof(uint8*));
}

void PdbParser::Initialize(const uint8* pdb, const size_t& size)
{
  Instance = new PdbParser(pdb,size);
  Instance->ParseDebugInfo();
}

uint32 PdbParser::PdbByteSize() const
{
  return _size;
}

uint32 PdbParser::PdbPageSize() const
{
  return BytesToPages(_size);
}

const uint8* PdbParser::Stream(uint32 sid) const
{
  // If already in the cache, return the buffer
  if(!_streamsCache[sid]) 
  {
    // Number of pages of this stream
    const uint32 streamPageSize = StreamPageSize(sid);

    // Create the stream cache
    _streamsCache[sid] = new uint8[streamPageSize * _header->PageSize];
    
    // Get the pointer into the pages table
    const uint32* pages = StreamPages(sid);

    // Copy page by page
    uint8* dst = _streamsCache[sid];
    for( uint32 i = 0; i < streamPageSize; i++ )
    {
      memcpy(dst, (const intptr)(_pdb + PageToOffset(pages[i])), _header->PageSize);
      dst += _header->PageSize;
    }
  }
  return _streamsCache[sid];
}

uint32 PdbParser::StreamByteSize(uint32 sid) const
{
  return _streamSizes[sid];
}

uint32 PdbParser::StreamPageSize(uint32 sid) const 
{
  return BytesToPages(_streamSizes[sid]);
}

const uint32* PdbParser::StreamPages(uint32 sid) const
{
  // Compute the offset in the stream pages table
  uint32 offset = 0;
  for( uint32 i = 0; i < sid; i++ )
  {
    offset += BytesToPages(_streamSizes[i]);
  }

  return _streamPages + offset;
}

uint32 PdbParser::NbStreams() const
{
  return _nbStreams;
}


uint32 PdbParser::PageToOffset(uint32 page) const
{
  return page * _header->PageSize;
}

uint32 PdbParser::BytesToPages(uint32 bytes) const
{
  return (bytes + _header->PageSize - 1) / _header->PageSize;
}

void PdbParser::ParseTypes()
{
  //const uint8* typeStream = Stream(2);
}


const SortedArray<PdbParser::PublicSymbolEntry*>& PdbParser::Symbols() const
{
  return _symbols;
}

int32 PdbParser::CompareEntries(PublicSymbolEntry* const & a, PublicSymbolEntry* const & b)
{
  if(a->Segment < b->Segment) return -1;
  if(a->Segment > b->Segment) return 1;

  if(a->Offset < b->Offset) return -1;
  if(a->Offset > b->Offset) return 1;
  return 0;
}

void PdbParser::ParseDebugInfo()
{
  const uint8* dbiStream = Stream(3);

  if( ((uint32*)dbiStream)[0] != 0xFFFFFFFF || ((uint32*)dbiStream)[1] != 19990903 )
    return;

  uint32 symPage = *(uint16*)(dbiStream + 0x14);

  const uint8* symStream = Stream(symPage);
  uint32 streamSize = StreamByteSize(symPage);

  while(streamSize)
  {
    Record* record = (Record*)symStream;
    if(record->Type == 0x110E)
    {
      _symbols.Insert((PublicSymbolEntry*)record);
    }
    const uint16 recordSize = record->Size + 2;
    symStream += recordSize;
    streamSize -= recordSize;
  }
}

const PdbParser::PublicSymbolEntry* PdbParser::GetSymbol(uint16 seg, uint32 off) const
{
  PublicSymbolEntry dummyEntry;
  dummyEntry.Size = 12;
  dummyEntry.Type = 0x110E;
  dummyEntry.Flags = 2;
  dummyEntry.Segment = seg;
  dummyEntry.Offset = off;
  dummyEntry.Name[0] = 0;
  
  uint32 index = _symbols.Find(&dummyEntry);

  if(index>=_symbols.Size()) return _symbols[_symbols.Size() - 1];
  if(_symbols[index]->Offset == off) return _symbols[index];
  return _symbols[index-1];
}

const PdbParser::PublicSymbolEntry* PdbParser::GetSymbol(uint32 eip) const
{
  return GetSymbol(0x0001, eip - 0xC0001000);
}
