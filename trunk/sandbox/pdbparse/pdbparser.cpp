#include "StdAfx.h"
#include "PdbParser.h"

using namespace std;

#define PDB_SIGNATURE "Microsoft C/C++ MSF 7.00\r\n\x1A" "DS\0\0\0"
#define PDB_SIGNATURE_SIZE (0x20)

PdbParser::PdbParser(const uint8* pdb, const size_t& size)
  : _pdb(pdb), _size(size)
{
  Initialize();
}

void PdbParser::Initialize()
{
  if(_pdb==0 || _size==0)
  {
    cout << "No input data." << endl;
    return;
  }

  if( memcmp(_pdb, PDB_SIGNATURE, PDB_SIGNATURE_SIZE) )
  {
    cout << "Not a valid PDB 7 file." << endl;
    return;
  }

  _header = (Header*)(_pdb + PDB_SIGNATURE_SIZE);

  _root = (uint32*)(_pdb + PageToOffset(_header->RootPage-1));
  _nbStreams = *_root;
  _streamSizes = (uint32*)(_root + 1);
  _streamPages = (uint32*)(_streamSizes + _nbStreams);
  
  _streamsCache = new uint8*[_nbStreams];
  memset(_streamsCache, 0, _nbStreams * sizeof(uint8*));

  cout << "PDB contains " << _nbStreams << " streams" << endl;
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
      memcpy(dst, _pdb + PageToOffset(pages[i]), _header->PageSize);
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

