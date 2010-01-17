#include "Shell.h"

#include <System/IO/StandardStream.h>
#include <System/IO/StreamReader.h>
#include <System/IO/StreamWriter.h>

using namespace GenOS::Application::Shell;

void Shell::Main(int argc, char** argv)
{
  Shell sh;
  sh.Run();

  // Must hang because thread finalizer is not implemented yet...
  Kernel::Hang();
}

Shell::Shell(void)
{
  cin = new System::IO::StreamReader (System::IO::StdInStream::GetInstance());
  cout = new System::IO::StreamWriter (System::IO::StdOutStream::GetInstance());
}

Shell::~Shell(void)
{
}

void Shell::Run()
{
  cout->Write ("Hello GenOS");
}