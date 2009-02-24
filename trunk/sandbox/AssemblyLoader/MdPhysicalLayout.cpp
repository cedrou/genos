#include "Metadata.h"
#include "../../src/kernel/bitmanip.h"
#include <windows.h>
#include <winnt.h>


using namespace GenOS;


#define ASSERTMSG(test, msg) \
  if( (test)==false )               \
  {                                 \
    std::cout << msg << std::endl;  \
    return NULL;                   \
  }

static uint32 Rva2Offset(uint32 rva, PIMAGE_SECTION_HEADER sections, uint16 nbsec)
{
  for (uint16 sec=0; sec<nbsec; sec++)
  {
    if (rva >= sections->VirtualAddress && (rva < sections->VirtualAddress + sections->Misc.VirtualSize) )
    {
      return rva - sections->VirtualAddress + sections->PointerToRawData;
    }
    sections++;
  }
  return 0;
}

//==================================================
// MdPhysicalLayout
//==================================================

MdPhysicalLayout::MdPhysicalLayout()
{
  mdRoot = NULL;
  mdHeader = NULL;
  for( int i = 0; i < streamCount; i++ )
  {
    mdStreams[i].Offset = 0;
    mdStreams[i].Size = 0;
  }
  mdTableHeader = NULL;
}

MdPhysicalLayout* MdPhysicalLayout::Initialize(const uint8* pefile)
{
  PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pefile;
  ASSERTMSG( dos->e_magic == IMAGE_DOS_SIGNATURE, "Not a MZ file." );
  
  PIMAGE_NT_HEADERS32 nt32 = (PIMAGE_NT_HEADERS32)(pefile + dos->e_lfanew);
  ASSERTMSG( nt32->Signature == IMAGE_NT_SIGNATURE, "Not a PE file." );

  ASSERTMSG( nt32->OptionalHeader.NumberOfRvaAndSizes >= IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR, "Not a .Net assembly." );
  PIMAGE_DATA_DIRECTORY dir = &nt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR];
  ASSERTMSG( dir->VirtualAddress != 0, "Not a .Net assembly." );
  ASSERTMSG( dir->Size != 0, "Not a .Net assembly." );

  PIMAGE_SECTION_HEADER sections = IMAGE_FIRST_SECTION(nt32);

  uint32 netSectionOffset = Rva2Offset(dir->VirtualAddress, sections, nt32->FileHeader.NumberOfSections);

  PIMAGE_COR20_HEADER netSection = (PIMAGE_COR20_HEADER)(pefile + netSectionOffset);

  uint32 mdOffset = Rva2Offset(netSection->MetaData.VirtualAddress, sections, nt32->FileHeader.NumberOfSections);


  if( ((MdRoot*)(pefile + mdOffset))->Signature != METADATA_MAGIC )
    return NULL;

  // Create a new instance
  MdPhysicalLayout* mdpl = new MdPhysicalLayout;

  mdpl->dos = dos;
  mdpl->nt32 = nt32;
  mdpl->sections = sections;
  mdpl->net = netSection;

  // Read headers
  mdpl->mdRoot = (MdRoot*)(pefile + mdOffset);
  mdpl->mdHeader = (MdRoot2*)((uint8*)(mdpl->mdRoot + 1) + mdpl->mdRoot->VersionLength);

  // Read stream info
  MdStreamHeader* currentStream = (MdStreamHeader*)(mdpl->mdHeader + 1);
  for( int i=0; i < mdpl->mdHeader->NbStreams; i++ )
  {
    MdStreamType type = streamCount;
    switch( *(uint16*)(currentStream + 1) )
    {
    case 'S#': type = streamString; break;
    case 'U#': type = streamUS; break;
    case 'B#': type = streamBlob; break;
    case 'G#': type = streamGuid; break;
    case '~#': type = streamTable; break;
    case '-#': type = streamNonOptTable; break;
    }
    if (type == streamCount) break;

    mdpl->mdStreams[type].Offset = currentStream->Offset;
    mdpl->mdStreams[type].Size = currentStream->Size;

    const uint8 nameSizes[streamCount] = { 12, 4, 8, 8, 4, 4 };
    currentStream = (MdStreamHeader*)((uint8*)(currentStream+1) + nameSizes[type]);
  }

  // Read metadata tables header
  mdpl->mdTableHeader = (MdTableHeader*)(mdpl->GetStream(streamTable));

  uint32  nbMdTables = BitManip::CountBitSet(mdpl->mdTableHeader->Valid);
  uint32* rows = (uint32*)(mdpl->mdTableHeader + 1);
  uint8*  record = (uint8*)(rows + nbMdTables);

  // First initialize counts and schemas
  uint64 valids = mdpl->mdTableHeader->Valid;
  for( uint32 tabid = tabidFirst; tabid < tabidCount; tabid++)
  {
    if( valids & 1 )
    {
      mdpl->mdTables[tabid].InitializeStep1((MdTableId)tabid, *rows);
      rows++;
    }
    else
    {
      mdpl->mdTables[tabid].InitializeStep1((MdTableId)tabid, 0);
    }

    valids >>= 1;
  };

  // Then initialize sizes and records
  valids = mdpl->mdTableHeader->Valid;
  for( uint32 tabid = tabidFirst; tabid < tabidCount; tabid++)
  {
    if( valids & 1 )
    {
      mdpl->mdTables[tabid].InitializeStep2(record, mdpl);
      record += mdpl->mdTables[tabid].RecordCount() * mdpl->mdTables[tabid].RecordSize();
    }
    else
    {
      mdpl->mdTables[tabid].InitializeStep2(NULL, mdpl);
    }
    valids >>= 1;
  };

  return mdpl;
}

const uint8* MdPhysicalLayout::GetStream(MdStreamType type) const
{
  return (uint8*)mdRoot + mdStreams[type].Offset;
}

const uint8* MdPhysicalLayout::GetString(uint32 index) const
{
  return GetStream(streamString) + index; 
}

const uint16* MdPhysicalLayout::GetUserString(uint32 index) const
{
  return (uint16*)GetStream(streamUS) + index; 
}

const uint8* MdPhysicalLayout::GetBlob(uint32 index) const
{
  return GetStream(streamBlob) + index; 
}

const uint8* MdPhysicalLayout::GetGuid(uint32 index) const
{
  return GetStream(streamGuid) + 16*(index - 1); 
}

const uint32 MdPhysicalLayout::GetImageBase() const
{
  return ((PIMAGE_NT_HEADERS32)nt32)->OptionalHeader.ImageBase;
}

const uint32 MdPhysicalLayout::GetFileAlignment() const
{
  return ((PIMAGE_NT_HEADERS32)nt32)->OptionalHeader.FileAlignment;
}

const uint32 MdPhysicalLayout::GetStackReserve() const
{
  return ((PIMAGE_NT_HEADERS32)nt32)->OptionalHeader.SizeOfStackReserve;
}

const uint32 MdPhysicalLayout::GetSubsystem() const
{
  return ((PIMAGE_NT_HEADERS32)nt32)->OptionalHeader.Subsystem;
}

const uint32 MdPhysicalLayout::GetCorFlags() const
{
  return ((PIMAGE_COR20_HEADER)net)->Flags;
}

const uint8* MdPhysicalLayout::GetMethod(uint32 rva) const
{
  return (uint8*)dos + Rva2Offset(rva, (PIMAGE_SECTION_HEADER)sections, ((PIMAGE_NT_HEADERS32)nt32)->FileHeader.NumberOfSections);
}
