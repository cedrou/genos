// pdbparse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PdbParser.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
  FILE* f = _tfopen(argv[1], _T("rb"));
  if(f==0) 
  {
    cout << "Cannot open file !" << endl;
    return 1;
  }

  fseek(f,0,SEEK_END);
  long size = ftell(f);
  fseek(f,0,SEEK_SET);
  if(size==0) 
  {
    cout << "0 byte file !" << endl;
    return 1;
  }

  uint8* pBuffer = new uint8[size];
  fread(pBuffer,1,size,f);
  fclose(f);

  PdbParser pdb(pBuffer,size);
  
  // Extract each stream in file "stream.###"
  for(uint32 sid = 0; sid < pdb.NbStreams(); sid++ )
  {
    const uint8* stream1 = pdb.Stream(sid);
    TCHAR out[32] = {0};
    _stprintf(out, _T("stream.%03i"), sid);
    f = _tfopen(out, _T("wb"));
    fwrite(stream1, 1, pdb.StreamByteSize(sid), f);
    fclose(f);
  }

  // Dump the list of pages used by each stream
  // Dump the list of unreferences pages
  f = _tfopen(_T("pages"), _T("wt"));
  uint8* unusedpages = new uint8[pdb.PdbPageSize()];
  memset(unusedpages, 1, pdb.PdbPageSize());
  for(uint32 sid = 0; sid < pdb.NbStreams(); sid++ )
  {
    const uint32* pages = pdb.StreamPages(sid);
    const uint32 streamPageSize = pdb.StreamPageSize(sid);

    _ftprintf(f, _T("stream.%03i: "), sid);
    for( uint32 i = 0; i < streamPageSize; i++ )
    {
      unusedpages[pages[i]] = 0;
      _ftprintf(f, _T("%02x "), pages[i]);
    }
    _ftprintf(f, _T("\n"));
  }
  _ftprintf(f, _T("unrefpages: "));
  for(uint32 i = 0; i < pdb.PdbPageSize(); i++ )
  {
    if(unusedpages[i])
      _ftprintf(f, _T("%02x "), i);
  }
  fclose(f);

	return 0;
}

