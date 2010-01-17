#include "StreamWriter.h"

using namespace GenOS::System::IO;

StreamWriter::StreamWriter(Stream* stream)
{
  _stream = stream;
  ASSERT (_stream);
  ASSERT (_stream->CanWrite());
}

StreamWriter::~StreamWriter()
{
}

void StreamWriter::Write (char c)
{
  _stream->WriteByte ((uint8)c);
}

void StreamWriter::Write (const char* c)
{
  while (*c)
    _stream->WriteByte ((uint8)*c++);
}
