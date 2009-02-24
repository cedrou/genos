#include "Metadata.h"

using namespace GenOS;

//=============================================
// MdToken
//=============================================

MdToken::MdToken(uint32 token)
{
  _token = token;
}

MdToken::MdToken(MdTableId tabid, uint32 index)
{
  _token = (tabid << 24) | (index & 0x00FFFFFF);
}

MdToken::MdToken(MdRecordItemType type, uint32 codix)
{
  CodedIndex ci = MdCodedIndex::Decode(type, codix);
  _token = (ci.tabid << 24) | (ci.index & 0x00FFFFFF);
}

MdToken::MdToken(const MdRecordItem& item)
{
  if(item.Type >= recItemCodedIndex)
  {
    CodedIndex ci = MdCodedIndex::Decode(item);
    _token = (ci.tabid << 24) | (ci.index & 0x00FFFFFF);
  }
  else if(item.Type >= recItemTableIndex)
  {
    _token = ((item.Type - recItemTableIndex) << 24) | (item.Value & 0x00FFFFFF);
  }
  else
  {
    throw;
  }
}

MdTableId MdToken::TabID() const
{
  return (MdTableId)(_token >> 24);
}

uint32 MdToken::Index() const
{
  return _token & 0x00FFFFFF;
}

uint32 MdToken::Token() const
{
  return _token;
}

MdRecord MdToken::Record(const MdTable* tables) const
{
  return tables[TabID()].GetRecord(Index());
}

MdToken MdToken::Max(const MdTable* tables) const
{
  return MdToken(TabID(), tables[TabID()].RecordCount() + 1);
}

MdToken MdToken::operator+(uint32 i) const
{
  return MdToken(_token + i);
}

MdToken& MdToken::operator++()
{
  _token++;
  return *this;
}

bool MdToken::operator==(const MdToken& tk) const
{
  return _token == tk._token;
}
bool MdToken::operator!=(const MdToken& tk) const
{
  return _token != tk._token;
}
