#pragma once

#include "../../src/kernel/common.h"

namespace GenOS {

class AssemblyLoader
{
public:
  AssemblyLoader();
  ~AssemblyLoader();

  bool Load(const uint8* pefile);
};

}