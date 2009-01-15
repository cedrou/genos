#pragma once

typedef unsigned __int32  uint32;
typedef unsigned char     uint8;

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

private:
  const uint8*  _pdb;
  const size_t  _size;
  const Header* _header;

  const uint32* _root;
  uint32        _nbStreams;
  const uint32* _streamSizes;
  const uint32* _streamPages;

  uint8**       _streamsCache;

public:
  PdbParser(const uint8* pdb, const size_t& size);

        uint32  PdbByteSize() const;
        uint32  PdbPageSize() const;

  const uint8*  Stream(uint32 sid) const;
        uint32  StreamByteSize(uint32 sid) const;
        uint32  StreamPageSize(uint32 sid) const;
  const uint32* StreamPages(uint32 sid) const;
        uint32  NbStreams() const;

private:
        uint32  PageToOffset(uint32 page) const;
        uint32  BytesToPages(uint32 bytes) const;

  void Initialize();
};
