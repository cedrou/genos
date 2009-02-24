#include "Metadata.h"

using namespace GenOS;


//==================================================
// MdRecord
//==================================================

MdRecord::MdRecord(const uint8* data, const MdRecordItemType* schema, const uint8* offsets)
: _data(data), _schema(schema), _offsets(offsets)
{
  _count = 0;
  while( *schema++ != recItemVOID ) _count++;
}

MdRecordItem MdRecord::GetItem(uint32 item, MdPhysicalLayout* mdpl) const
{
  return MdRecordItem(_schema[item], _data + _offsets[item], mdpl);
}
