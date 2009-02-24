#include "AssemblyLoader.h"
#include <iostream>
#include "Metadata.h"

using namespace GenOS;
#pragma warning(disable:4200)

AssemblyLoader::AssemblyLoader()
{
}

AssemblyLoader::~AssemblyLoader()
{
}

bool AssemblyLoader::Load(const uint8* pefile)
{
  MdPhysicalLayout* mdpl = MdPhysicalLayout::Initialize(pefile);
  if( mdpl == NULL )
  {
    std::cout << "Invalid .Net assembly." << std::endl;
    return false;
  }

  //MdTable::DumpTables(mdpl);
  MdDisassembler dasm(mdpl);
  dasm.Disassemble(std::cout);

  return true;
}