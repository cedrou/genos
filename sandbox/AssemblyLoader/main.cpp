
#include <iostream>
#include <tchar.h>
#include "AssemblyLoader.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
  FILE* f = _tfopen(argv[1], _T("rb"));
  if(f==0) 
  {
    cout << "Cannot open file !" << (const char*)argv[1] << endl;
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

  GenOS::AssemblyLoader asmldr;
  asmldr.Load(pBuffer);

  delete[] pBuffer;

  return 0;
}