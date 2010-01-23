#pragma once

#include <common.h>

// from winnt.h

struct IMAGE_FILE_HEADER 
{
  uint16  Machine;
  uint16  NumberOfSections;
  uint32  TimeDateStamp;
  uint32  PointerToSymbolTable;
  uint32  NumberOfSymbols;
  uint16  SizeOfOptionalHeader;
  uint16  Characteristics;
};

struct IMAGE_DATA_DIRECTORY 
{
  uint32   VirtualAddress;
  uint32   Size;
};

struct IMAGE_OPTIONAL_HEADER32
{
  //
  // Standard fields.
  //

  uint16  Magic;
  uint8   MajorLinkerVersion;
  uint8   MinorLinkerVersion;
  uint32  SizeOfCode;
  uint32  SizeOfInitializedData;
  uint32  SizeOfUninitializedData;
  uint32  AddressOfEntryPoint;
  uint32  BaseOfCode;
  uint32  BaseOfData;

  //
  // NT additional fields.
  //

  uint32  ImageBase;
  uint32  SectionAlignment;
  uint32  FileAlignment;
  uint16  MajorOperatingSystemVersion;
  uint16  MinorOperatingSystemVersion;
  uint16  MajorImageVersion;
  uint16  MinorImageVersion;
  uint16  MajorSubsystemVersion;
  uint16  MinorSubsystemVersion;
  uint32  Win32VersionValue;
  uint32  SizeOfImage;
  uint32  SizeOfHeaders;
  uint32  CheckSum;
  uint16  Subsystem;
  uint16  DllCharacteristics;
  uint32  SizeOfStackReserve;
  uint32  SizeOfStackCommit;
  uint32  SizeOfHeapReserve;
  uint32  SizeOfHeapCommit;
  uint32  LoaderFlags;
  uint32  NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[16];
};

struct IMAGE_NT_HEADERS32
{
    uint32 Signature;                       // Magic number 'PE00' = 0x00004550
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
};

// DOS .EXE header
struct IMAGE_DOS_HEADER 
{
  uint16  e_magic;     // Magic number 'MZ' = 0x5A4D
  uint16  e_cblp;      // Bytes on last page of file
  uint16  e_cp;        // Pages in file
  uint16  e_crlc;      // Relocations
  uint16  e_cparhdr;   // Size of header in paragraphs
  uint16  e_minalloc;  // Minimum extra paragraphs needed
  uint16  e_maxalloc;  // Maximum extra paragraphs needed
  uint16  e_ss;        // Initial (relative) SS value
  uint16  e_sp;        // Initial SP value
  uint16  e_csum;      // Checksum
  uint16  e_ip;        // Initial IP value
  uint16  e_cs;        // Initial (relative) CS value
  uint16  e_lfarlc;    // File address of relocation table
  uint16  e_ovno;      // Overlay number
  uint16  e_res[4];    // Reserved words
  uint16  e_oemid;     // OEM identifier (for e_oeminfo)
  uint16  e_oeminfo;   // OEM information; e_oemid specific
  uint16  e_res2[10];  // Reserved words
  uint32  e_lfanew;    // File address of new exe header
};
