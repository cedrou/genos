#pragma once

namespace GenOS {
  namespace System {
    namespace IO {

class TextReader
{
public:
  virtual char Read () = 0;
};

    }
  }
}