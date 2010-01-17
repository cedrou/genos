#include "StreamReader.h"

using namespace GenOS::System::IO;

StreamReader::StreamReader(Stream* stream)
{
  _stream = stream;
  ASSERT (_stream);
  ASSERT (_stream->CanRead());
}

StreamReader::~StreamReader()
{
}

char StreamReader::Read ()
{
  return (char)_stream->ReadByte ();
}
