#include "Metadata.h"

using namespace GenOS;


//==================================================
// MdRecordItem
//==================================================

MdRecordItem::MdRecordItem(MdRecordItemType type, const uint8* data, MdPhysicalLayout* mdpl)
{
  Type = type;

  switch(MdTable::GetRecordItemSize(type, mdpl))
  {
  case 1: Value = *data; break;
  case 2: Value = *(uint16*)data; break;
  case 4: Value = *(uint32*)data; break;
  }

}

