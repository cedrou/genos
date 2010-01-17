#pragma once

#include <System/IO/TextWriter.h>
#include <System/IO/Stream.h>

namespace GenOS {
  namespace System {
    namespace IO {

      class StreamWriter : public TextWriter
      {
      private:
        Stream* _stream;

      public:
        StreamWriter(Stream* stream);
        ~StreamWriter();

        virtual void Write (char c);
        virtual void Write (const char* c);
        //virtual void Write (const char* c, size_t index, size_t count);
        //virtual void Write (const String& string);
      };

    }
  }
}