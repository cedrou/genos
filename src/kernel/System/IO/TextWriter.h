#pragma once

namespace GenOS {
  namespace System {
    namespace IO {

class TextWriter
{
public:
  virtual void Write (char c) = 0;
  virtual void Write (const char* c) = 0;
};

    }
  }
}