#pragma once

#include "../common.h"
#include "../fifo.h"
#include "../registers.h"

namespace GenOS {
  namespace Driver {

class Keyboard
{
private:
  Fifo<uint32, 256> _cache;

public:
  Keyboard();
  ~Keyboard();

private:
  static void __stdcall InterruptHandler(const Registers& regs, void* data);
};

  }
}
