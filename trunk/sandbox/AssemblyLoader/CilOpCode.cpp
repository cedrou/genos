#include "Metadata.h"

using namespace GenOS;

const CilOpCode cilOpCode_FE[] = 
{
  { 0x00, NULL, "arglist",      false, 2 },
  { 0x01, NULL, "ceq",          false, 2 },
  { 0x02, NULL, "cgt",          false, 2 },
  { 0x03, NULL, "cgt.un",       false, 2 },
  { 0x04, NULL, "clt",          false, 2 },
  { 0x05, NULL, "clt.un",       false, 2 },
  { 0x06, NULL, "ldftn",        false, 2 },
  { 0x07, NULL, "ldvirtftn",    false, 2 },
    
  { 0x09, NULL, "ldarg",        false, 2 },
  { 0x0A, NULL, "ldarga",       false, 2 },
  { 0x0B, NULL, "starg",        false, 2 },
  { 0x0C, NULL, "ldloc",        false, 2 },
  { 0x0D, NULL, "ldloca",       false, 2 },
  { 0x0E, NULL, "stloc",        false, 2 },
  { 0x0F, NULL, "localloc",     false, 2 },
    
  { 0x11, NULL, "endfilter",    false, 2 },
  { 0x12, NULL, "unaligned.",   true, 2 },
  { 0x13, NULL, "volatile.",    true, 2 },
  { 0x14, NULL, "tail.",        true, 2 },
  { 0x15, NULL, "initobj",      false, 2 },
  { 0x16, NULL, "constrained.", true, 2 },
  { 0x17, NULL, "cpblk",        false, 2 },
  { 0x18, NULL, "initblk",      false, 2 },
  { 0x19, NULL, "no.",          true, 2 },
  { 0x1A, NULL, "rethrow",      false, 2 },
    
  { 0x1C, NULL, "sizeof",       false, 2 },
  { 0x1D, NULL, "refanytype",   false, 2 },
  { 0x1E, NULL, "readonly.",    true, 2 },

  { 0x00, NULL, NULL, false, 2 },
};

const CilOpCode cilOpCodes[] = 
{
  { 0x00, NULL, "nop",        false, 1,  0 },
  { 0x01, NULL, "break",      false, 1 },
  { 0x02, NULL, "ldarg.0",    false, 1,  0 },
  { 0x03, NULL, "ldarg.1",    false, 1 },
  { 0x04, NULL, "ldarg.2",    false, 1 },
  { 0x05, NULL, "ldarg.3",    false, 1 },
  { 0x06, NULL, "ldloc.0",    false, 1 },
  { 0x07, NULL, "ldloc.1",    false, 1 },
  { 0x08, NULL, "ldloc.2",    false, 1 },
  { 0x09, NULL, "ldloc.3",    false, 1 },
  { 0x0A, NULL, "stloc.0",    false, 1 },
  { 0x0B, NULL, "stloc.1",    false, 1 },
  { 0x0C, NULL, "stloc.2",    false, 1 },
  { 0x0D, NULL, "stloc.3",    false, 1 },
  { 0x0E, NULL, "ldarg.s",    false, 1 },
  { 0x0F, NULL, "ldarga.s",   false, 1 },
  { 0x10, NULL, "starg.s",    false, 1 },
  { 0x11, NULL, "ldloc.s",    false, 1 },
  { 0x12, NULL, "ldloca.s",   false, 1 },
  { 0x13, NULL, "stloc.s",    false, 1 },
  { 0x14, NULL, "ldnull",     false, 1 },
  { 0x15, NULL, "ldc.i4.m1",  false, 1 },
  { 0x16, NULL, "ldc.i4.0",   false, 1 },
  { 0x17, NULL, "ldc.i4.1",   false, 1 },
  { 0x18, NULL, "ldc.i4.2",   false, 1 },
  { 0x19, NULL, "ldc.i4.3",   false, 1 },
  { 0x1A, NULL, "ldc.i4.4",   false, 1 },
  { 0x1B, NULL, "ldc.i4.5",   false, 1 },
  { 0x1C, NULL, "ldc.i4.6",   false, 1 },
  { 0x1D, NULL, "ldc.i4.7",   false, 1 },
  { 0x1E, NULL, "ldc.i4.8",   false, 1 },
  { 0x1F, NULL, "ldc.i4.s",   false, 1 },
  { 0x20, NULL, "ldc.i4",     false, 1 },
  { 0x21, NULL, "ldc.i8",     false, 1 },
  { 0x22, NULL, "ldc.r4",     false, 1 },
  { 0x23, NULL, "ldc.r8",     false, 1 },
  { 0x25, NULL, "dup",        false, 1 },
  { 0x26, NULL, "pop",        false, 1 },
  { 0x27, NULL, "jmp",        false, 1 },
  { 0x28, NULL, "call",       false, 1, 1 },
  { 0x29, NULL, "calli",      false, 1 },
  { 0x2A, NULL, "ret",        false, 1 },
  { 0x2B, NULL, "br.s",       false, 1 },
  { 0x2C, NULL, "brfalse.s",  false, 1 },
  { 0x2D, NULL, "brtrue.s",   false, 1 },
  { 0x2E, NULL, "beq.s",      false, 1 },
  { 0x2F, NULL, "bge.s",      false, 1 },
  { 0x30, NULL, "bgt.s",      false, 1 },
  { 0x31, NULL, "ble.s",      false, 1 },
  { 0x32, NULL, "blt.s",      false, 1 },
  { 0x33, NULL, "bne.un.s",   false, 1 },
  { 0x34, NULL, "bge.un.s",   false, 1 },
  { 0x35, NULL, "bgt.un.s",   false, 1 },
  { 0x36, NULL, "ble.un.s",   false, 1 },
  { 0x37, NULL, "blt.un.s",   false, 1 },
  { 0x38, NULL, "br",         false, 1 },
  { 0x39, NULL, "brfalse",    false, 1 },
  { 0x3A, NULL, "brtrue",     false, 1 },
  { 0x3B, NULL, "beq",        false, 1 },
  { 0x3C, NULL, "bge",        false, 1 },
  { 0x3D, NULL, "bgt",        false, 1 },
  { 0x3E, NULL, "ble",        false, 1 },
  { 0x3F, NULL, "blt",        false, 1 },
  { 0x40, NULL, "bne.un",     false, 1 },
  { 0x41, NULL, "bge.un",     false, 1 },
  { 0x42, NULL, "bgt.un",     false, 1 },
  { 0x43, NULL, "ble.un",     false, 1 },
  { 0x44, NULL, "blt.un",     false, 1 },
  { 0x45, NULL, "switch",     false, 1 },
  { 0x46, NULL, "ldind.i1",   false, 1 },
  { 0x47, NULL, "ldind.u1",   false, 1 },
  { 0x48, NULL, "ldind.i2",   false, 1 },
  { 0x49, NULL, "ldind.u2",   false, 1 },
  { 0x4A, NULL, "ldind.i4",   false, 1 },
  { 0x4B, NULL, "ldind.u4",   false, 1 },
  { 0x4C, NULL, "ldind.i8",   false, 1 },
  { 0x4D, NULL, "ldind.i",    false, 1 },
  { 0x4E, NULL, "ldind.r4",   false, 1 },
  { 0x4F, NULL, "ldind.r8",   false, 1 },
  { 0x50, NULL, "ldind.ref",  false, 1 },
  { 0x51, NULL, "stind.ref",  false, 1 },
  { 0x52, NULL, "stind.i1",   false, 1 },
  { 0x53, NULL, "stind.i2",   false, 1 },
  { 0x54, NULL, "stind.i4",   false, 1 },
  { 0x55, NULL, "stind.i8",   false, 1 },
  { 0x56, NULL, "stind.r4",   false, 1 },
  { 0x57, NULL, "stind.r8",   false, 1 },
  { 0x58, NULL, "add",        false, 1 },
  { 0x59, NULL, "sub",        false, 1 },
  { 0x5A, NULL, "mul",        false, 1 },
  { 0x5B, NULL, "div",        false, 1 },
  { 0x5C, NULL, "div.un",     false, 1 },
  { 0x5D, NULL, "rem",        false, 1 },
  { 0x5E, NULL, "rem.un",     false, 1 },
  { 0x5F, NULL, "and",        false, 1 },
  { 0x60, NULL, "or",         false, 1 },
  { 0x61, NULL, "xor",        false, 1 },
  { 0x62, NULL, "shl",        false, 1 },
  { 0x63, NULL, "shr",        false, 1 },
  { 0x64, NULL, "shr.un",     false, 1 },
  { 0x65, NULL, "neg",        false, 1 },
  { 0x66, NULL, "not",        false, 1 },
  { 0x67, NULL, "conv.i1",    false, 1 },
  { 0x68, NULL, "conv.i2",    false, 1 },
  { 0x69, NULL, "conv.i4",    false, 1 },
  { 0x6A, NULL, "conv.i8",    false, 1 },
  { 0x6B, NULL, "conv.r4",    false, 1 },
  { 0x6C, NULL, "conv.r8",    false, 1 },
  { 0x6D, NULL, "conv.u4",    false, 1 },
  { 0x6E, NULL, "conv.u8",    false, 1 },
  { 0x6F, NULL, "callvirt",   false, 1 },
  { 0x70, NULL, "cpobj",      false, 1 },
  { 0x71, NULL, "ldobj",      false, 1 },
  { 0x72, NULL, "ldstr",      false, 1, 1 },
  { 0x73, NULL, "newobj",     false, 1 },
  { 0x74, NULL, "castclass",  false, 1 },
  { 0x75, NULL, "isinst",     false, 1 },
  { 0x76, NULL, "conv.r.un",  false, 1 },


  { 0x79, NULL, "unbox", false, 1 },
  { 0x7A, NULL, "throw", false, 1 },
  { 0x7B, NULL, "ldfld", false, 1 },
  { 0x7C, NULL, "ldflda", false, 1 },
  { 0x7D, NULL, "stfld", false, 1 },
  { 0x7E, NULL, "ldsfld", false, 1 },
  { 0x7F, NULL, "ldsflda", false, 1 },
  { 0x80, NULL, "stsfld", false, 1 },
  { 0x81, NULL, "stobj", false, 1 },
  { 0x82, NULL, "conv.ovf.i1.un", false, 1 },
  { 0x83, NULL, "conv.ovf.i2.un", false, 1 },
  { 0x84, NULL, "conv.ovf.i4.un", false, 1 },
  { 0x85, NULL, "conv.ovf.i8.un", false, 1 },
  { 0x86, NULL, "conv.ovf.u1.un", false, 1 },
  { 0x87, NULL, "conv.ovf.u2.un", false, 1 },
  { 0x88, NULL, "conv.ovf.u4.un", false, 1 },
  { 0x89, NULL, "conv.ovf.u8.un", false, 1 },
  { 0x8A, NULL, "conv.ovf.i.un", false, 1 },
  { 0x8B, NULL, "conv.ovf.u.un", false, 1 },
  { 0x8C, NULL, "box", false, 1 },
  { 0x8D, NULL, "newarr", false, 1 },
  { 0x8E, NULL, "ldlen", false, 1 },
  { 0x8F, NULL, "ldelema", false, 1 },
  { 0x90, NULL, "ldelem.i1", false, 1 },
  { 0x91, NULL, "ldelem.u1", false, 1 },
  { 0x92, NULL, "ldelem.i2", false, 1 },
  { 0x93, NULL, "ldelem.u2", false, 1 },
  { 0x94, NULL, "ldelem.i4", false, 1 },
  { 0x95, NULL, "ldelem.u4", false, 1 },
  { 0x96, NULL, "ldelem.i8", false, 1 },
  { 0x97, NULL, "ldelem.i", false, 1 },
  { 0x98, NULL, "ldelem.r4", false, 1 },
  { 0x99, NULL, "ldelem.r8", false, 1 },
  { 0x9A, NULL, "ldelem.ref", false, 1 },
  { 0x9B, NULL, "stelem.i", false, 1 },
  { 0x9C, NULL, "stelem.i1", false, 1 },
  { 0x9D, NULL, "stelem.i2", false, 1 },
  { 0x9E, NULL, "stelem.i4", false, 1 },
  { 0x9F, NULL, "stelem.i8", false, 1 },
  { 0xA0, NULL, "stelem.r4", false, 1 },
  { 0xA1, NULL, "stelem.r8", false, 1 },
  { 0xA2, NULL, "stelem.ref", false, 1 },
  { 0xA3, NULL, "ldelem", false, 1 },
  { 0xA4, NULL, "stelem", false, 1 },
  { 0xA5, NULL, "unbox.any", false, 1 },













  { 0xB3, NULL, "conv.ovf.i1", false, 1 },
  { 0xB4, NULL, "conv.ovf.u1", false, 1 },
  { 0xB5, NULL, "conv.ovf.i2", false, 1 },
  { 0xB6, NULL, "conv.ovf.u2", false, 1 },
  { 0xB7, NULL, "conv.ovf.i4", false, 1 },
  { 0xB8, NULL, "conv.ovf.u4", false, 1 },
  { 0xB9, NULL, "conv.ovf.i8", false, 1 },
  { 0xBA, NULL, "conv.ovf.u8", false, 1 },







  { 0xC2, NULL, "refanyval", false, 1 },
  { 0xC3, NULL, "ckfinite", false, 1 },


  { 0xC6, NULL, "mkrefany", false, 1 },









  { 0xD0, NULL, "ldtoken", false, 1 },
  { 0xD1, NULL, "conv.u2", false, 1 },
  { 0xD2, NULL, "conv.u1", false, 1 },
  { 0xD3, NULL, "conv.i", false, 1 },
  { 0xD4, NULL, "conv.ovf.i", false, 1 },
  { 0xD5, NULL, "conv.ovf.u", false, 1 },
  { 0xD6, NULL, "add.ovf", false, 1 },
  { 0xD7, NULL, "add.ovf.un", false, 1 },
  { 0xD8, NULL, "mul.ovf", false, 1 },
  { 0xD9, NULL, "mul.ovf.un", false, 1 },
  { 0xDA, NULL, "sub.ovf", false, 1 },
  { 0xDB, NULL, "sub.ovf.un", false, 1 },
  { 0xDC, NULL, "endfinally", false, 1 },
  { 0xDD, NULL, "leave", false, 1 },
  { 0xDE, NULL, "leave.s", false, 1 },
  { 0xDF, NULL, "stind.i", false, 1 },
  { 0xE0, NULL, "conv.u", false, 1 },

  { 0xFE, cilOpCode_FE, NULL, 1 },

  { 0x00, NULL, NULL, false, 1 },
};

const CilOpCode* CilOpCode::Find(const uint8* cil)
{
  const CilOpCode* opCode = &cilOpCodes[0];

  while( opCode->name )
  {
    if( opCode->opcode!=*cil )
    {
      opCode++;
      continue;
    }

    if( opCode->next )
    {
      opCode = opCode->next;
      cil++;
      continue;
    }

    return opCode;
  };

  return NULL;
}
