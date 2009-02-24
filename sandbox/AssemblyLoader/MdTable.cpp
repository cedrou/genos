#include "Metadata.h"
#include <iostream>
#include <iomanip>

using namespace GenOS;


static const char* typesNames[recItemCount] = 
{
  "",
  "UINT8",
  "UINT16","", 
  "UINT32","","","","","","","","","","","", 

  "",
  "STRING",
  "GUID","",
  "BLOB","","","","","","","","","","","",

  "","",
  "TYPEDEF","",
  "FIELD","",
  "METHODDEF","",
  "PARAM","","","","","","","","","","","",
  "EVENT","","",
  "PROPERTY","","",
  "MODULEREF","","","","","","","","",
  "ASSEMBLYREF","","","","","","",
  "GENERICPARAM","","","","","",

  "",
  "TYPEDEFORREF",
  "HASCONSTANT",
  "HASCUSTOMATTRIBUTE",
  "HASFIELDMARSHALL",
  "HASDECLSECURITY",
  "MEMBERREFPARENT",
  "HASSEMANTICS",
  "METHODDEFORREF",
  "MEMBERFORWARDED",
  "IMPLEMENTATION",
  "CUSTOMATTRIBUTETYPE",
  "RESOLUTIONSCOPE",
  "TYPEORMETHODDEF",
};

const char* MdTable::Names[tabidCount] = 
{
  "Module",
  "TypeRef",
  "TypeDef",
  "",
  "Field",
  "",
  "MethodDef",
  "",
  "Param",
  "InterfaceImpl",
  "MemberRef",
  "Constant",
  "CustomAttribute",
  "FieldMarshal",
  "DeclSecurity",
  "ClassLayout",

  "FieldLayout",
  "StandAloneSig",
  "EventMap",
  "",
  "Event",
  "PropertyMap",
  "",
  "Property",
  "MethodSemantics",
  "MethodImpl",
  "ModuleRef",
  "TypeSpec",
  "ImplMap",
  "FieldRVA",
  "",
  "",

  "Assembly",
  "AssemblyProcessor",
  "AssemblyOS",
  "AssemblyRef",
  "AssemblyRefProcessor",
  "AssemblyRefOS",
  "File",
  "ExportedType",
  "ManifestResource",
  "NestedClass",
  "GenericParam",
  "MethodSpec",
  "GenericParamConstraint",
};


//==================================================
// MdTable
//==================================================

MdTable::MdTable()
{
  _recordSize = 0;
  _count = 0;
  _firstRecord = NULL;
  for( uint8 i=0; i<9; i++ ) _offsets[i]= 0;
}

void MdTable::InitializeStep1(MdTableId tabid, uint32 count)
{
  _tabid = tabid;
  _count = count;
}
void MdTable::InitializeStep2(const uint8* start, MdPhysicalLayout* mdpl)
{
  _recordSize = GetRecordSize(_tabid, mdpl);
  
  const MdRecordItemType* sch = _schemas[_tabid];
  uint8* offset = _offsets;
  uint8 current = 0;
  while(*sch!=recItemVOID)
  {
    *offset = current;
    current += GetRecordItemSize(*sch, mdpl);
    sch++;
    offset++;
  }

  _firstRecord = start;
}

MdRecord MdTable::GetRecord(uint32 index) const
{
  return MdRecord( _firstRecord + (index-1) * _recordSize, _schemas[_tabid], _offsets );
}

uint8 MdTable::_itemSizes[recItemCount] = {0};

uint32 MdTable::GetRecordItemSize(MdRecordItemType type, MdPhysicalLayout* mdpl)
{
  if (_itemSizes[type] == 0)
  {
    if (type >= recItemCodedIndex)
    {
      _itemSizes[type] = 2;

      const uint32 index = (type - recItemCodedIndex);
      for( uint8 i=0; i < MdCodedIndex::_codedIndexTags[index].Size; i++ )
      {
        const MdTableId tabid = MdCodedIndex::_codedIndexTags[index].Tags[i];
        const uint32 maxRows = (uint32)0xFFFF >> MdCodedIndex::_codedIndexTags[index].Bits;

        if(tabid!=tabidCount)
        {
          const uint32 count = mdpl->mdTables[tabid]._count;
          if ( count > maxRows )
          {
            _itemSizes[type] += 2;
            break;
          }
        }
      }
    }
    else if (type >= recItemTableIndex)
    {
      _itemSizes[type] = 2;
      if (mdpl->mdTables[(type - recItemTableIndex)]._count > 65535)
        _itemSizes[type] += 2; 
    }
    else if (type >= recItemHeapIndex)
    {
      _itemSizes[type] = 2;
      if (mdpl->mdTableHeader->HeapSizes & (type - recItemHeapIndex)) 
        _itemSizes[type] += 2;
    }
    else if (type >= recItemConstant)
    {
      _itemSizes[type] = (type - recItemConstant);
    }
  }

  return _itemSizes[type];
}

uint32 MdTable::GetRecordSize(MdTableId tabid, MdPhysicalLayout* mdpl)
{
  uint32 size = 0;
  const MdRecordItemType* schema = _schemas[tabid];
  if(schema==NULL) return 0;
  while (*schema != recItemVOID)
  {
    size += GetRecordItemSize(*schema, mdpl);
    schema++;
  }
  return size;
}

void MdTable::Dump(MdPhysicalLayout* mdpl)
{
  const uint8* currentRecord = _firstRecord;
  for( uint32 row=1; row<=_count; row++ )
  {
    std::cout << "  Record #" << std::setw(4) << std::setfill('0') << std::hex << row << std::endl;

    MdRecord record = GetRecord(row);
    for( uint32 item = 0; item < record.Count(); item++ )
    {
      MdRecordItem recitem = record.GetItem(item, mdpl);

      switch(GetRecordItemSize(recitem.Type, mdpl))
      {
      case 1:
        std::cout << std::setw(12) << std::setfill(' ') << "0x" << std::setw(2) << std::setfill('0') << std::hex << recitem.Value;
        break;
      case 2:
        std::cout << std::setw(10) << std::setfill(' ') << "0x" << std::setw(4) << std::setfill('0') << std::hex << recitem.Value;
        break;
      case 4:
        std::cout << std::setw(6) << std::setfill(' ') << "0x" << std::setw(8) << std::setfill('0') << std::hex << recitem.Value;
        break;
      }

      std::cout << " " << std::setw(20) << std::setfill(' ') << typesNames[recitem.Type]; 

      switch (recitem.Type)
      {
      case recItemUINT8:
      case recItemUINT16:
      case recItemUINT32:
        std::cout << " " << std::setw(0) << std::dec << recitem.Value; 
        break;

      case recItemSTRING:
        std::cout << " \"" << (char*)(mdpl->GetStream(streamString) + recitem.Value) << "\"";
        break;

      case recItemGUID:
        {
          std::cout << " {";
          const uint8* guid = mdpl->GetStream(streamGuid) + 16*recitem.Value;
          for( uint8 i=0; i<16; i++ )
          {
            std::cout << " " << std::setw(2) << std::setfill('0') << std::hex << (uint32)*guid++;
          }
          std::cout << "}";
        }
        break;

      case recItemBLOB:
        {
          std::cout << " {";
          const uint8* blob = mdpl->GetStream(streamBlob) + recitem.Value;
          const uint8 size = *blob++;
          for( uint8 i=0; i<size; i++ )
          {
            std::cout << " " << std::setw(2) << std::setfill('0') << std::hex << (uint32)*blob++;
          }
          std::cout << " }";
        }
        break;

      case recItemTYPEDEF:
      case recItemFIELD:
      case recItemMETHODDEF:
      case recItemPARAM:
      case recItemEVENT:
      case recItemPROPERTY:
      case recItemMODULEREF:
      case recItemASSEMBLYREF:
      case recItemGENERICPARAM:

      case recItemTYPEDEFORREF:
      case recItemHASCONSTANT:
      case recItemHASCUSTOMATTRIBUTE:
      case recItemHASFIELDMARSHALL:
      case recItemHASDECLSECURITY:
      case recItemMEMBERREFPARENT:
      case recItemHASSEMANTICS:
      case recItemMETHODDEFORREF:
      case recItemMEMBERFORWARDED:
      case recItemIMPLEMENTATION:
      case recItemCUSTOMATTRIBUTETYPE:
      case recItemRESOLUTIONSCOPE:
      case recItemTYPEORMETHODDEF:
        break;
      }

      std::cout << std::endl;
    }

  }
}

void MdTable::DumpTables(MdPhysicalLayout* mdpl)
{
  for( int tabid=tabidFirst; tabid<tabidCount; tabid++ )
  {
    std::cout << "Table #" << std::setw(2) << std::setfill('0') << std::hex << tabid;
    std::cout << " " << Names[tabid] << std::endl;
    mdpl->mdTables[tabid].Dump(mdpl);
  }
}
