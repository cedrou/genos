#pragma once

#include <System/IO/TextReader.h>
#include <System/IO/Stream.h>

namespace GenOS {
  namespace System {
    namespace IO {

      class StreamReader : public TextReader
      {
      private:
        Stream* _stream;

      public:
        StreamReader(Stream* stream);
        ~StreamReader();

        virtual char Read ();
      };

    }
  }
}
