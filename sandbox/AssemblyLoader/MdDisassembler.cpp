#include "Metadata.h"
#include <iomanip>

using namespace GenOS;

enum ElementType
{
  ELEMENT_TYPE_END          = 0x00, // Marks end of a list
  ELEMENT_TYPE_VOID         = 0x01,
  ELEMENT_TYPE_BOOLEAN      = 0x02,
  ELEMENT_TYPE_CHAR         = 0x03,
  ELEMENT_TYPE_I1           = 0x04,
  ELEMENT_TYPE_U1           = 0x05,
  ELEMENT_TYPE_I2           = 0x06,
  ELEMENT_TYPE_U2           = 0x07,
  ELEMENT_TYPE_I4           = 0x08,
  ELEMENT_TYPE_U4           = 0x09,
  ELEMENT_TYPE_I8           = 0x0a,
  ELEMENT_TYPE_U8           = 0x0b,
  ELEMENT_TYPE_R4           = 0x0c,
  ELEMENT_TYPE_R8           = 0x0d,
  ELEMENT_TYPE_STRING       = 0x0e,
  ELEMENT_TYPE_PTR          = 0x0f, // Followed by type
  ELEMENT_TYPE_BYREF        = 0x10, // Followed by type
  ELEMENT_TYPE_VALUETYPE    = 0x11, // Followed by TypeDef or TypeRef token
  ELEMENT_TYPE_CLASS        = 0x12, // Followed by TypeDef or TypeRef token
  ELEMENT_TYPE_VAR          = 0x13, // Generic parameter in a generic type definition, represented as number
  ELEMENT_TYPE_ARRAY        = 0x14, // type rank boundsCount bound1 … loCount lo1 …
  ELEMENT_TYPE_GENERICINST  = 0x15, // Generic type instantiation. Followed by type typearg-count type-1 ... type-n
  ELEMENT_TYPE_TYPEDBYREF   = 0x16,
  ELEMENT_TYPE_I            = 0x18, // System.IntPtr
  ELEMENT_TYPE_U            = 0x19, // System.UIntPtr
  ELEMENT_TYPE_FNPTR        = 0x1b, // Followed by full method signature
  ELEMENT_TYPE_OBJECT       = 0x1c, // System.Object
  ELEMENT_TYPE_SZARRAY      = 0x1d, // Single-dim array with 0 lower bound
  ELEMENT_TYPE_MVAR         = 0x1e, // Generic parameter in a generic method definition, represented as number
  ELEMENT_TYPE_CMOD_REQD    = 0x1f, // Required modifier : followed by a TypeDef or TypeRef token
  ELEMENT_TYPE_CMOD_OPT     = 0x20, // Optional modifier : followed by a TypeDef or TypeRef token
  ELEMENT_TYPE_INTERNAL     = 0x21, // Implemented within the CLI
  ELEMENT_TYPE_MODIFIER     = 0x40, // Or’d with following element types
  ELEMENT_TYPE_SENTINEL     = 0x41, // Sentinel for vararg method signature
  ELEMENT_TYPE_PINNED       = 0x45, // Denotes a local variable that points at a pinned object
                          //= 0x50, // Indicates an argument of type System.Type.
                          //= 0x51, // Used in custom attributes to specify a boxed object (§23.3).
                          //= 0x52, // Reserved
                          //= 0x53, // Used in custom attributes to indicate a FIELD (§22.10, 23.3).
                          //= 0x54, // Used in custom attributes to indicate a PROPERTY (§22.10, 23.3).
                          //= 0x55, // Used in custom attributes to specify an enum (§23.3).
};


//=============================================
// MdDisassembler
//=============================================

MdDisassembler::MdDisassembler(MdPhysicalLayout* mdpl)
: _mdpl(mdpl), _tables(mdpl->mdTables)
{
}

void MdDisassembler::Disassemble(std::ostream& os) const
{
  ExternModules(os);
  ExternAssemblies(os);
  Assembly(os);
  Module(os);
  
  os << std::endl;
  os << "// =============== CLASS MEMBERS DECLARATION ===================";

  Classes(os);
}

void MdDisassembler::ExternModules(std::ostream& os) const
{
  for( uint32 i=0; i < _tables[tabidModuleRef].RecordCount(); i++ )
  {
    MdRecord record = _tables[tabidModuleRef].GetRecord(i + 1);

    os << ".module extern "; String(os, record.GetItem(0, _mdpl).Value) << std::endl;
  }
}

void MdDisassembler::ExternAssemblies(std::ostream& os) const
{
  for( uint32 i=1; i <= _tables[tabidAssemblyRef].RecordCount(); i++ )
  {
    MdRecord record = _tables[tabidAssemblyRef].GetRecord(i);

    os << ".assembly extern "; String(os, record.GetItem(6, _mdpl).Value) << std::endl;
    os << "{" << std::endl;
    os << "  .publickeytoken = ("; Blob(os, record.GetItem(5, _mdpl).Value) << ")" << std::endl;
    os << "  .ver " << record.GetItem(0, _mdpl).Value <<
                ":" << record.GetItem(1, _mdpl).Value <<
                ":" << record.GetItem(2, _mdpl).Value <<
                ":" << record.GetItem(3, _mdpl).Value << std::endl;
    os << "}" << std::endl;
  }
}

std::vector<MdRecord> MdDisassembler::FindCustomAttributes(const MdToken& tk) const
{
  std::vector<MdRecord> attr;

  MdToken tkCurrent(tabidCustomAttribute, 1);
  MdToken tkMax = tkCurrent.Max(_tables);
  while ( tkCurrent != tkMax )
  {
    MdRecord customAttributeRecord = tkCurrent.Record(_tables);

    MdToken tkParent(customAttributeRecord.GetItem(0, _mdpl));
    if( tkParent == tk )
    {
      attr.push_back(customAttributeRecord);
    }

    ++tkCurrent;
  }

  return attr;
}

MdRecord MdDisassembler::FindConstant(const MdToken& tk) const
{
  MdToken tkCurrent(tabidConstant, 1);
  MdToken tkMax = tkCurrent.Max(_tables);
  while ( tkCurrent != tkMax )
  {
    MdRecord constantRecord = tkCurrent.Record(_tables);

    MdToken tkParent(constantRecord.GetItem(2, _mdpl));
    if( tkParent == tk )
    {
      return constantRecord;
    }

    ++tkCurrent;
  }
}

MdToken MdDisassembler::FindTypeDef(const MdToken& tk) const
{
  MdToken tkCurrent(tabidTypeDef, 2);
  MdToken tkMax = tkCurrent.Max(_tables);
  switch (tk.TabID())
  {
  case tabidField:
    while ( tkCurrent != tkMax )
    {
      MdRecord typedefRecord = tkCurrent.Record(_tables);

      MdToken tkField(typedefRecord.GetItem(4, _mdpl));
      if( tkField > tk )
      {
        return tkCurrent - 1;
      }

      ++tkCurrent;
    }
    break;

  case tabidMethodDef:
    while ( tkCurrent != tkMax )
    {
      MdRecord typedefRecord = tkCurrent.Record(_tables);

      MdToken tkMethodDef(typedefRecord.GetItem(5, _mdpl));
      if( tkMethodDef > tk )
      {
        return tkCurrent - 1;
      }

      ++tkCurrent;
    }
    break;
  }
}

void MdDisassembler::Assembly(std::ostream& os) const
{
  // Zero or one record
  if(_tables[tabidAssembly].RecordCount() == 0) return;
  
  MdToken tkAssembly(tabidAssembly, 1);
  MdRecord record = tkAssembly.Record(_tables);

  os << ".assembly "; String(os, record.GetItem(7, _mdpl).Value) << std::endl;
  os << "{" << std::endl;

  // Enumerate each custom attribute
  std::vector<MdRecord> attr = FindCustomAttributes(tkAssembly);

  for( uint32 i=0; i < attr.size(); i++ )
  {
    CustomAttribute(os, attr[i]) << std::endl;
  }

  os << "  .hash algorithm 0x"; Hex32(os, record.GetItem(0, _mdpl).Value) << std::endl;
  os << "  .ver " << record.GetItem(1, _mdpl).Value <<
              ":" << record.GetItem(2, _mdpl).Value <<
              ":" << record.GetItem(3, _mdpl).Value <<
              ":" << record.GetItem(4, _mdpl).Value << std::endl;
  os << "}" << std::endl;
}

void MdDisassembler::Module(std::ostream& os) const
{
  MdRecord modulerec = _tables[tabidModule].GetRecord(1);

  os << ".module "; String(os, modulerec.GetItem(1, _mdpl).Value) << std::endl;
  os << "// MVID: "; Guid(os, modulerec.GetItem(2, _mdpl).Value) << std::endl;
  os << ".imagebase 0x"; Hex32(os, _mdpl->GetImageBase()) << std::endl;
  os << ".file alignment 0x"; Hex32(os, _mdpl->GetFileAlignment()) << std::endl;
  os << ".stackreserve 0x"; Hex32(os, _mdpl->GetStackReserve()) << std::endl;
  os << ".subsystem 0x"; Hex16(os, _mdpl->GetSubsystem()) << std::endl;
  os << ".corflags 0x"; Hex32(os, _mdpl->GetCorFlags()) << std::endl;
  os << "// Image base: 0x"; Hex32(os, _mdpl->GetImageBase()) << std::endl;
}


void MdDisassembler::Classes(std::ostream& os) const
{
  // TODO: The first row of the TypeDef table represents the pseudo class that acts 
  // as parent for functions and variables defined at module scope. (<Module>)
#define VisibilityMask 0x00000007
#define LayoutMask 0x00000018
#define ClassSemanticsMask 0x00000020
#define Abstract 0x00000080

  MdToken tkCurrent(tabidTypeDef, 1);
  MdToken tkMax = tkCurrent.Max(_tables);
  while ( tkCurrent != tkMax )
  {
    os << std::endl;
    TypeDef_Impl(os, tkCurrent);
    ++tkCurrent;
  }

}



inline std::ostream& MdDisassembler::Hex(std::ostream& os, uint32 value) const
{
  //if(value <= 0XFF) return Hex8(os,value);
  //if(value <= 0XFFFF) return Hex16(os,value);
  //return Hex32(os,value);
  os << std::hex << std::uppercase << value << std::dec;
  return os;
}

inline std::ostream& MdDisassembler::Hex8(std::ostream& os, uint32 value) const
{
  os << std::setw(2) << std::setfill('0') << std::right << std::hex << std::uppercase << value << std::dec;
  return os;
}

inline std::ostream& MdDisassembler::Hex16(std::ostream& os, uint32 value) const
{
  os << std::setw(4) << std::setfill('0') << std::right << std::hex << std::uppercase  << value << std::dec;
  return os;
}

inline std::ostream& MdDisassembler::Hex32(std::ostream& os, uint32 value) const
{
  os << std::setw(8) << std::setfill('0') << std::right << std::hex << std::uppercase  << value << std::dec;
  return os;
}

inline std::ostream& MdDisassembler::hex(std::ostream& os, uint32 value) const
{
  //if(value <= 0XFF) return hex8(os,value);
  //if(value <= 0XFFFF) return hex16(os,value);
  //return hex32(os,value);
  os << std::hex;
  os.unsetf(std::ios_base::uppercase);
  os << value << std::dec;
  return os;
}

inline std::ostream& MdDisassembler::hex8(std::ostream& os, uint32 value) const
{
  os << std::setw(2) << std::setfill('0') << std::right << std::hex;
  os.unsetf(std::ios_base::uppercase);
  os << value << std::dec;
  return os;
}

inline std::ostream& MdDisassembler::hex16(std::ostream& os, uint32 value) const
{
  os << std::setw(4) << std::setfill('0') << std::right << std::hex;
  os.unsetf(std::ios_base::uppercase);
  os << value << std::dec;
  return os;
}

inline std::ostream& MdDisassembler::hex32(std::ostream& os, uint32 value) const
{
  os << std::setw(8) << std::setfill('0') << std::right << std::hex;
  os.unsetf(std::ios_base::uppercase);
  os << value << std::dec;
  return os;
}

inline std::ostream& MdDisassembler::String(std::ostream& os, uint32 index) const
{
  os << _mdpl->GetString(index);
  return os;
}

inline std::ostream& MdDisassembler::UserString(std::ostream& os, uint32 index) const
{
  const uint8* us = _mdpl->GetUserString(index);
  const uint32 size = DecodeSignatureItem(us);
  const uint32 nbchar = size / 2;
  os << "\"";
  for( uint32 i=0; i<nbchar; i++ )
  {
    uint16 uchar = *(uint16*)us;
    os << (uint8)uchar;
    us += 2;
  }
  os << "\"";
  return os;
}

inline std::ostream& MdDisassembler::Blob(std::ostream& os, uint32 index) const
{
  const uint8* blob = _mdpl->GetBlob(index);
  const uint32 size = DecodeSignatureItem(blob);
  for( uint8 i=0; i<size; i++ )
  {
    Hex8(os, *blob++) << " " ;
  }
  return os;
}

inline std::ostream& MdDisassembler::Guid(std::ostream& os, uint32 index) const
{
  const uint8* guid = _mdpl->GetGuid(index);
  os << "{"; 
  Hex32(os, *(uint32*)guid);
  guid += 4;

  os << "-";
  Hex16(os, *(uint16*)guid); 
  guid += 2;

  os << "-";
  Hex16(os, *(uint16*)guid);
  guid += 2;

  os << "-";
  Hex8(os, *guid++); Hex8(os, *guid++); 
  os << "-";
 
  Hex8(os, *guid++); Hex8(os, *guid++); Hex8(os, *guid++); Hex8(os, *guid++); Hex8(os, *guid++); Hex8(os, *guid++); 

  os << "}"; 

  return os;
}

std::ostream& MdDisassembler::CustomAttribute(std::ostream& os, const MdRecord& record) const
{
  os << "  .custom ";

  MdToken tkType(record.GetItem(1, _mdpl));
  switch(tkType.TabID())
  {
  case tabidMethodDef:
    os << "???MethodDef???";
    break;

  case tabidMemberRef:
    MemberRef(os, tkType);
    break;
  }

  os << " = ( "; Blob(os, record.GetItem(2, _mdpl).Value) << ")";

  return os;
}

std::ostream& MdDisassembler::TypeDefOrRef(std::ostream& os, const MdToken& token) const
{
  switch(token.TabID())
  {
  case tabidTypeDef:  return TypeDef(os, token);
  case tabidTypeRef:  return TypeRef(os, token);
  case tabidTypeSpec: return TypeSpec(os, token);
  }
  return os;
}

std::ostream& MdDisassembler::TypeDef(std::ostream& os, const MdToken& token) const
{
  MdRecord recTypeDef = token.Record(_tables);

  // Namespace.Name
  if( recTypeDef.GetItem(2, _mdpl).Value ) String(os, recTypeDef.GetItem(2, _mdpl).Value) << ".";
  String(os, recTypeDef.GetItem(1, _mdpl).Value);

  return os;
}

std::ostream& MdDisassembler::TypeDef_Impl(std::ostream& os, const MdToken& token) const
{
  MdToken tkMax = token.Max(_tables);
  MdToken tkNext = token + 1;

  MdRecord recTypeDef = token.Record(_tables);

  const uint32 flags = recTypeDef.GetItem(0, _mdpl).Value;
  // Filter out <Module> ?
  if( flags != 0 )
  {
    //Class semantics attributes
    switch( flags & ClassSemanticsMask )
    {
    case 0x00000000: os << ".class "; break;
    case 0x00000020: os << ".interface "; break;
    }

    // Visibility attributes
    switch( flags & VisibilityMask )
    {
    case 0x00: os << "private "; break;
    case 0x01: os << "public "; break;
    case 0x02: os << "???Visibility_2??? "; break;
    case 0x03: os << "???Visibility_3??? "; break;
    case 0x04: os << "???Visibility_4??? "; break;
    case 0x05: os << "???Visibility_5??? "; break;
    case 0x06: os << "???Visibility_6??? "; break;
    case 0x07: os << "???Visibility_7??? "; break;
    }

    if( flags & 0x00000080 ) os << "abstract "; 
    // Class layout attributes
    switch( flags & LayoutMask )
    {
    case 0x00000000: os << "auto "; break;
    case 0x00000008: os << "sequential "; break;
    case 0x00000010: os << "explicit "; break;
    }

    // Special semantics in addition to class semantics
    if( flags & 0x00000400 ) os << "specialname "; 

    // Implementation Attributes
    if( flags & 0x00001000 ) os << "import "; 
    if( flags & 0x00002000 ) os << "serializable "; 

    // String formatting Attributes
    switch( flags & 0x00030000 )
    {
    case 0x00000000: os << "ansi "; break;
    case 0x00010000: os << "unicode "; break;
    case 0x00020000: os << "auto "; break;
    case 0x00030000: os << "custom "; break;
    }

    if( flags & 0x00000100 ) os << "sealed "; 

    // Class Initialization Attributes
    if( flags & 0x00100000 ) os << "beforefieldinit "; 

    // Additional Flags
    if( flags & 0x00000800 ) os << "rtspecialname "; 
    if( flags & 0x00040000 ) os << "hassecurity "; 

    // Namespace.Name
    if( recTypeDef.GetItem(2, _mdpl).Value ) String(os, recTypeDef.GetItem(2, _mdpl).Value) << ".";
    String(os, recTypeDef.GetItem(1, _mdpl).Value) << std::endl;

    // Extends
    os << "       extends "; TypeDefOrRef(os, MdToken(recTypeDef.GetItem(3, _mdpl))) << std::endl;

    os << "{" << std::endl;

    // Fields
    MdToken tkField(recTypeDef.GetItem(4, _mdpl));
    MdToken tkFieldMax = tkField.Max(_tables);
    if( tkNext != tkMax )
    {
      MdRecord recNext = tkNext.Record(_tables);
      tkFieldMax = MdToken(recNext.GetItem(4, _mdpl));
    }

    while(tkField != tkFieldMax)
    {
      Field_Impl(os, tkField) << std::endl;
      ++tkField;
    }

    // Methods
    MdToken tkMethod(recTypeDef.GetItem(5, _mdpl));
    MdToken tkMethodMax = tkMethod.Max(_tables);
    if( tkNext != tkMax )
    {
      MdRecord recNext = tkNext.Record(_tables);
      tkMethodMax = MdToken(recNext.GetItem(5, _mdpl));
    }

    while(tkMethod != tkMethodMax)
    {
      MethodDef_Impl(os, tkMethod) << std::endl;
      ++tkMethod;
    }

    os << "} // end of class ";
    if( recTypeDef.GetItem(2, _mdpl).Value ) String(os, recTypeDef.GetItem(2, _mdpl).Value) << ".";
    String(os, recTypeDef.GetItem(1, _mdpl).Value) << std::endl;
  }
  return os;
}
std::ostream& MdDisassembler::TypeRef(std::ostream& os, const MdToken& token) const
{
  const MdRecord& record = token.Record(_tables);

  // ResolutionScope
  MdToken tkScope(record.GetItem(0, _mdpl));
  MdRecord recScope = tkScope.Record(_tables);
  switch(tkScope.TabID())
  {
  case tabidModule:
    break;
  case tabidModuleRef:
    break;
  case tabidAssemblyRef:
    os << "["; String(os, recScope.GetItem(6,_mdpl).Value) << "]";
    break;
  case tabidTypeRef:
    TypeRef(os, tkScope) << "/";
    break;
  }

  // Namespace
  if(record.GetItem(2, _mdpl).Value) 
    String(os, record.GetItem(2, _mdpl).Value) << ".";

  // Type
  String(os, record.GetItem(1, _mdpl).Value);

  return os;
}

std::ostream& MdDisassembler::TypeSpec(std::ostream& os, const MdToken& token) const
{
  const MdRecord& record = token.Record(_tables);

  // Get Member signature
  const uint8* signature = _mdpl->GetBlob(record.GetItem(0, _mdpl).Value);
  const uint8 sigsize = *signature++;

  // first item should be 0X06
  SignatureElementType(os, signature);


  return os;
}

std::ostream& MdDisassembler::MemberRef(std::ostream& os, const MdToken& token) const
{
  const MdRecord& record = token.Record(_tables);

  // Get Member signature
  const uint8* signature = _mdpl->GetBlob(record.GetItem(2, _mdpl).Value);
  const uint8 sigsize = *signature++;

  // Calling convention
  if( *signature & 0x20 /*HASTHIS*/ )
  {
    os << "instance ";
    if( *signature & 0x40 /*EXPLICITTHIS*/ )
    {
      os << "explicit ";
    }
  }
       if( *signature & 0x01 ) os << "unmanaged cdecl ";
  else if( *signature & 0x02 ) os << "unmanaged fastcall ";
  else if( *signature & 0x03 ) os << "unmanaged stdcall ";
  else if( *signature & 0x04 ) os << "unmanaged thiscall ";
  else if( *signature & 0x04 ) os << "vararg ";
  signature++;

  // Param count
  uint8 paramcount = DecodeSignatureItem(signature);

  // Return type
  SignatureElementType(os, signature) << " ";

  // Parent of the member reference
  MdToken tkParent(record.GetItem(0, _mdpl));
  switch( tkParent.TabID() )
  {
  case tabidTypeDef:
    break;

  case tabidTypeRef:
    TypeRef(os, tkParent) << "::";
    break;

  case tabidModuleRef:
  case tabidMethodDef:
  case tabidTypeSpec:
    break;
  }

  // Name
  String(os, record.GetItem(1, _mdpl).Value);

  // Parameters types
  os << "(";
  while(paramcount--)
  {
    SignatureElementType(os, signature);
    if(paramcount) os << " ";
  }
  os << ")";

  return os;
}

std::ostream& MdDisassembler::Field(std::ostream& os, const MdToken& token) const
{
  const MdRecord& recField = token.Record(_tables);

  // Get Member signature
  const uint8* signature = _mdpl->GetBlob(recField.GetItem(2, _mdpl).Value);
  const uint8 sigsize = *signature++;

  // first item should be 0X06
  signature++;
  SignatureElementType(os, signature) << " ";

  // Parent of the member reference
  MdToken tkParent = FindTypeDef(token);
  TypeDef(os, tkParent) << "::";

  // Name
  String(os, recField.GetItem(1, _mdpl).Value);

  return os;
}

std::ostream& MdDisassembler::Field_Impl(std::ostream& os, const MdToken& token) const
{
  const MdRecord& recField = token.Record(_tables);

  os << "  .field ";

  // Flags
  const uint32 flags = recField.GetItem(0, _mdpl).Value;

  switch( flags & 0x0007 )
  {
  case 0x00: os << "???Visibility_0??? "; break;
  case 0x01: os << "private "; break;
  case 0x02: os << "???Visibility_2??? "; break;
  case 0x03: os << "???Visibility_3??? "; break;
  case 0x04: os << "family "; break;
  case 0x05: os << "???Visibility_5??? "; break;
  case 0x06: os << "public "; break;
  }

  if( flags & 0x0010 ) os << "static "; 
  if( flags & 0x0020 ) os << "initonly "; 
  if( flags & 0x0040 ) os << "literal "; 
  if( flags & 0x0080 ) os << "notserialized ";  // reserved
  if( flags & 0x0200 ) os << "specialname "; 
  if( flags & 0x2000 ) os << "pinvokeimpl "; 
  if( flags & 0x0400 ) os << "rtspecialname "; 
  if( flags & 0x1000 ) os << "hasfieldmarshal "; 
  if( flags & 0x0100 ) os << "hasfieldrva "; 


  // Get Member signature
  const uint8* signature = _mdpl->GetBlob(recField.GetItem(2, _mdpl).Value);
  const uint8 sigsize = *signature++;

  // first item should be 0X06
  signature++;

  // Type
  SignatureElementType(os, signature) << " ";

  // Name
  String(os, recField.GetItem(1, _mdpl).Value);

  if( flags & 0x8000 ) // os << "hasdefault "; 
  {
    os << " = ";
    MdRecord constantRecord = FindConstant(token);
    Blob(os, constantRecord.GetItem(3, _mdpl).Value);
  }

  std::vector<MdRecord> attr = FindCustomAttributes(token);
  for( uint32 i=0; i < attr.size(); i++ )
  {
    os << std::endl;
    CustomAttribute(os, attr[i]);
  }

  return os;
}

std::ostream& MdDisassembler::MethodDef_Def(std::ostream& os, const MdToken& token) const
{
  const MdRecord& recMethod = token.Record(_tables);

  // Get Member signature
  const uint8* signature = _mdpl->GetBlob(recMethod.GetItem(4, _mdpl).Value);
  const uint8 sigsize = *signature++;

  // Calling convention
  if( *signature & 0x20 /*HASTHIS*/ )
  {
    os << "instance ";
    if( *signature & 0x40 /*EXPLICITTHIS*/ )
    {
      os << "explicit ";
    }
  }
       if( *signature & 0x01 ) os << "unmanaged cdecl ";
  else if( *signature & 0x02 ) os << "unmanaged fastcall ";
  else if( *signature & 0x03 ) os << "unmanaged stdcall ";
  else if( *signature & 0x04 ) os << "unmanaged thiscall ";
  else if( *signature & 0x04 ) os << "vararg ";
  signature++;

  // Param count
  uint8 paramcount = DecodeSignatureItem(signature);

  // Return type
  SignatureElementType(os, signature) << " ";

  // Parent of the member reference
  MdToken tkParent = FindTypeDef(token);
  TypeDef(os, tkParent) << "::";

  // Name
  String(os, recMethod.GetItem(3, _mdpl).Value);

  // Parameters
  os << "(";
  while(paramcount--)
  {
    SignatureElementType(os, signature);
    if(paramcount) os << " ";
  }
  os << ")";

  return os;
}

std::ostream& MdDisassembler::MethodDef_Impl(std::ostream& os, const MdToken& tkMethod) const
{
  const MdToken tkMethodNext = tkMethod + 1;
  const MdToken tkMethodMax = tkMethod.Max(_tables);

  const MdRecord& recMethod = tkMethod.Record(_tables);

  os << "  .method ";

  const uint32 flags = recMethod.GetItem(2, _mdpl).Value;

  // Member access
  switch( flags & 0x0007 )
  {
  case 0x01: os << "private "; break;
  case 0x06: os << "public "; break;
  case 0x00: os << "???Visibility_0??? "; break;
  case 0x02: os << "???Visibility_2??? "; break;
  case 0x03: os << "???Visibility_3??? "; break;
  case 0x04: os << "family "; break;
  case 0x05: os << "???Visibility_5??? "; break;
  case 0x07: os << "???Visibility_7??? "; break;
  }

  if( flags & 0x0080 ) os << "hidebysig "; 
  if( flags & 0x0010 ) os << "static "; 
  if( flags & 0x0020 ) os << "sealed "; 
  if( flags & 0x0040 ) os << "virtual "; 

  switch( flags & 0x0100 )
  {
  case 0x0000: break; // nothing to do
  case 0x0100: os << "newslot "; break;
  }

  if( flags & 0x0200 ) os << "strict "; 
  if( flags & 0x0400 ) os << "abstract "; 
  if( flags & 0x0800 ) os << "specialname "; 

  // Interop attributes
  if( flags & 0x2000 ) os << "pinvokeimpl "; 
  if( flags & 0x0008 ) os << "unmanagedexport "; 

  // Additional flags
  if( flags & 0x1000 ) os << "rtspecialname "; 
  if( flags & 0x4000 ) os << "hassecurity "; 
  if( flags & 0x8000 ) os << "requiresecobject "; 



  // Get Member signature
  const uint8* signature = _mdpl->GetBlob(recMethod.GetItem(4, _mdpl).Value);
  const uint8 sigsize = *signature++;

  // Calling convention
  if( *signature & 0x20 /*HASTHIS*/ )
  {
    os << "instance ";
    if( *signature & 0x40 /*EXPLICITTHIS*/ )
    {
      os << "explicit ";
    }
  }
       if( *signature & 0x01 ) os << "unmanaged cdecl ";
  else if( *signature & 0x02 ) os << "unmanaged fastcall ";
  else if( *signature & 0x03 ) os << "unmanaged stdcall ";
  else if( *signature & 0x04 ) os << "unmanaged thiscall ";
  else if( *signature & 0x04 ) os << "vararg ";
  signature++;

  // Param count
  uint8 paramcount = DecodeSignatureItem(signature);

  // Return type
  SignatureElementType(os, signature) << " ";

  // Name
  String(os, recMethod.GetItem(3, _mdpl).Value);

  // Parameters
  os << "(";

  MdToken tkParam(recMethod.GetItem(5, _mdpl));
  MdToken tkParamMax = tkParam.Max(_tables);
  if( tkMethodNext != tkMethodMax )
  {
    MdRecord recNext = tkMethodNext.Record(_tables);
    tkParamMax = MdToken(recNext.GetItem(5, _mdpl));
  }

  uint8 param = 1;
  while(param <= paramcount)
  {
    if(param != 1) os << ", ";

    SignatureElementType(os, signature);

    if(tkParam != tkParamMax)
    {
      MdRecord recParam = tkParam.Record(_tables);

      uint16 sequence = recParam.GetItem(1, _mdpl).Value;
      if( sequence == param )
      {
        uint16 flags = recParam.GetItem(0, _mdpl).Value;
        if (flags & 0x0001) os << "[In] ";
        if (flags & 0x0002) os << "[Out] ";
        if (flags & 0x0010) os << "[Optional] ";
        if (flags & 0x1000) os << "???HasDefault??? ";
        if (flags & 0x2000) os << "???HasFieldMarshal??? ";

        uint32 name = recParam.GetItem(2, _mdpl).Value;
        if(name)
        {
          os << " "; String(os, name);
        }

        ++tkParam;
      }
    }

    param++;
  }
  os << ") ";


  const uint32 implflags = recMethod.GetItem(1, _mdpl).Value;

  switch( implflags & 0x0003 )
  {
  case 0x0000: os << "cil "; break;
  case 0x0001: os << "native "; break;
  case 0x0002: os << "optil "; break;
  case 0x0003: os << "runtime "; break;
  }
  switch( implflags & 0x0004 )
  {
  case 0x0000: os << "managed "; break;
  case 0x0004: os << "unmanaged "; break;
  }
  if( implflags & 0x0010 ) os << "forwardref "; 
  if( implflags & 0x0080 ) os << "preservesig "; 
  if( implflags & 0x1000 ) os << "internal "; 
  if( implflags & 0x0020 ) os << "synchronized "; 
  if( implflags & 0x0008 ) os << "noinline "; 

  os << std::endl;
  os << "  {" << std::endl;

  if (_mdpl->GetEntryPoint() == tkMethod)
  {
    os << "    .entrypoint" << std::endl;
  }

  std::vector<MdRecord> attr = FindCustomAttributes(tkMethod);

  for( uint32 i=0; i < attr.size(); i++ )
  {
    os << "  "; CustomAttribute(os, attr[i]) << std::endl;
  }

  //os << "    //RVA: 0x"; Hex32(os, recMethod.GetItem(0, _mdpl).Value) << std::endl;
  const uint8* methodcil = _mdpl->GetMethod(recMethod.GetItem(0, _mdpl).Value);
  uint8  headersize = 0;
  uint32 cilsize = 0;
  uint32 maxstack = 0;
  uint32 locals = 0;
  if( (*methodcil & 0x03) == 0x02 )
  { // Tiny header
    headersize = 1;
    cilsize = (*methodcil >> 2);
    maxstack = 8;
  }
  else
  { // Fat header
    uint16 flags = (*(uint16*)methodcil ) >> 4;
    headersize = (*(uint16*)methodcil & 0x0F) * 4;
    maxstack = *(uint16*)(methodcil + 2);
    cilsize = *(uint32*)(methodcil + 4);
    locals = *(uint32*)(methodcil + 8);
  }

  os << "    // Code size " << cilsize << " (0x"; hex(os, cilsize) << ")" << std::endl;
  os << "    .maxstack " << maxstack << std::endl;

  if(locals)
  {
    // Get Locals signature
    const uint8* signature = _mdpl->GetBlob(MdToken(locals).Record(_tables).GetItem(0, _mdpl).Value);
    const uint8 sigsize = *signature++;

    // First item should be 0x07
    signature++;

    uint16 localcount = DecodeSignatureItem(signature);
    os << "    .locals init ("; 
    for( uint16 i=0; i<localcount; i++ )
    {
      if(i) os << "," << std::endl << "             ";
      os << "[" << i << "] ";
      SignatureElementType(os, signature);
    }
    os << ")" << std::endl;
  }

  methodcil += headersize;

  // Dump CIL bytes
  //os << "    // ";
  //const uint8* blob = methodcil;
  //for( uint8 i=0; i<cilsize + 1; i++ )
  //{
  //  Hex8(os, *blob++) << " " ;
  //}
  //os << std::endl;

  // Disassemble CIL
  const uint8* methodorg = methodcil;
  while(cilsize)
  {
    const CilOpCode* opcode = CilOpCode::Find(methodcil);
    if(opcode==NULL) break;

    // Offset
    os << "    IL_"; hex16(os, methodcil - methodorg) << ":  ";

    // OpCode
    os << std::setfill(' ') << std::setw(10) << std::left << opcode->name;
    methodcil += opcode->codebytes;
    cilsize -= opcode->codebytes;

    // Args
    switch (opcode->argtype)
    {
    case cilUINT8:
      os << " " << (uint32)*(uint8*)methodcil;
      methodcil += 1;
      cilsize -= 1;
      break;
    case cilUINT16:
      os << " 0x"; Hex16(os, *(uint16*)methodcil);
      methodcil += 2;
      cilsize -= 2;
      break;
    case cilUINT32:
      os << " 0x"; hex(os, *(uint32*)methodcil);
      methodcil += 4;
      cilsize -= 4;
      break;
    case cilR4:
      os << " " << *(float*)methodcil;
      methodcil += 4;
      cilsize -= 4;
      break;
    case cilR8:
      os << " " << *(double*)methodcil;
      methodcil += 8;
      cilsize -= 8;
      break;
    case cilTOKEN:
      os << " "; Token(os, MdToken(*(uint32*)methodcil));
      methodcil += 4;
      cilsize -= 4;
      break;
    case cilOFFSET8:
      os << " IL_"; hex16(os, *(uint8*)methodcil + (uint32)(methodcil - methodorg + 1)) << std::endl;
      methodcil += 1;
      cilsize -= 1;
      break;
    case cilOFFSET32:
      os << " 0x"; Hex32(os, *(uint32*)methodcil);
      methodcil += 4;
      cilsize -= 4;
      break;
    case cilNONE:
    default:
      break;
    }

    os << std::endl;
  }

  os << "  } // end of method ";
  TypeDef( os, FindTypeDef(tkMethod)) << "::";
  /*String(os, recTypeDef.GetItem(1, _mdpl).Value) << "::";*/ 
  String(os, recMethod.GetItem(3, _mdpl).Value) << std::endl;

  return os;
}

uint32 MdDisassembler::DecodeSignatureItem(const uint8*& signature) const
{
  uint32 value = 0;
  if( (*signature & 0xC0) == 0xC0 )
  {
    value += ((*signature++) << 24);
    value += ((*signature++) << 16);
    value += ((*signature++) << 8);
    value += ((*signature++) << 0);
    value &= 0x1FFFFFFF;
  }
  else if( (*signature & 0x80) == 0x80 )
  {
    value += ((*signature++) << 8);
    value += ((*signature++) << 0);
    value &= 0x3FFF;
  }
  else
  {
    value += ((*signature++) << 0);
    value &= 0x7F;
  }
  return value;
}

std::ostream& MdDisassembler::SignatureElementType(std::ostream& os, const uint8*& signature) const
{
  switch(DecodeSignatureItem(signature))
  {
  case ELEMENT_TYPE_END:
    break;

  case ELEMENT_TYPE_VOID: os << "void"; break;
  case ELEMENT_TYPE_BOOLEAN: os << "bool"; break;
  case ELEMENT_TYPE_CHAR: os << "char"; break;
  case ELEMENT_TYPE_OBJECT: os << "object"; break;
  case ELEMENT_TYPE_STRING: os << "string"; break;
  case ELEMENT_TYPE_R4: os << "float32"; break;
  case ELEMENT_TYPE_R8: os << "float64"; break;
  case ELEMENT_TYPE_I1: os << "int8"; break;
  case ELEMENT_TYPE_I2: os << "int16"; break;
  case ELEMENT_TYPE_I4: os << "int32"; break;
  case ELEMENT_TYPE_I8: os << "int64"; break;
  case ELEMENT_TYPE_I: os << "native int"; break;
  case ELEMENT_TYPE_U: os << "native unsigned int"; break;
  case ELEMENT_TYPE_TYPEDBYREF: os << "typedref"; break;
  case ELEMENT_TYPE_U1: os << "unsigned int8"; break;
  case ELEMENT_TYPE_U2: os << "unsigned int16"; break;
  case ELEMENT_TYPE_U4: os << "unsigned int32"; break;
  case ELEMENT_TYPE_U8: os << "unsigned int64"; break;

  case ELEMENT_TYPE_VALUETYPE:
    os << "valuetype ";
    TypeDefOrRef(os, MdToken(recItemTYPEDEFORREF, DecodeSignatureItem(signature)));
    break;

  case ELEMENT_TYPE_SZARRAY: 
    SignatureElementType(os, signature) << "[]";
    break;

  case ELEMENT_TYPE_CLASS:
    os << "class ";
    TypeDefOrRef(os, MdToken(recItemTYPEDEFORREF, DecodeSignatureItem(signature)));
    break;

  case ELEMENT_TYPE_GENERICINST: 
    {
      // Class / Valuetype ?
      SignatureElementType(os, signature);

      os << "<";
      uint32 argcount = DecodeSignatureItem(signature);
      for( uint32 i = 0; i<argcount; i++ )
      {
        if(i) os << ", ";
        SignatureElementType(os, signature);
      }
      os << "> ";
    }
    break;
  case ELEMENT_TYPE_PTR: os << "ELEMENT_TYPE_PTR"; break;
  case ELEMENT_TYPE_BYREF: os << "ELEMENT_TYPE_BYREF"; break;
  case ELEMENT_TYPE_VAR: os << "ELEMENT_TYPE_VAR"; break;
  case ELEMENT_TYPE_ARRAY: os << "ELEMENT_TYPE_ARRAY"; break;
  case ELEMENT_TYPE_FNPTR: os << "ELEMENT_TYPE_FNPTR"; break;
  case ELEMENT_TYPE_MVAR: os << "ELEMENT_TYPE_MVAR"; break;
  case ELEMENT_TYPE_CMOD_REQD: os << "ELEMENT_TYPE_CMOD_REQD"; break;
  case ELEMENT_TYPE_CMOD_OPT: os << "ELEMENT_TYPE_CMOD_OPT"; break;
  case ELEMENT_TYPE_INTERNAL: os << "ELEMENT_TYPE_INTERNAL"; break;
  case ELEMENT_TYPE_MODIFIER: os << "ELEMENT_TYPE_MODIFIER"; break;
  case ELEMENT_TYPE_SENTINEL: os << "ELEMENT_TYPE_SENTINEL"; break;
  case ELEMENT_TYPE_PINNED: os << "ELEMENT_TYPE_PINNED"; break;

  }
  return os;
}

std::ostream& MdDisassembler::Token(std::ostream& os, const MdToken& token) const
{
  //os << "["; Hex32(os, token.Token()) << "] ";
  switch (token.TabID())
  {
  case tabidTypeRef:    return TypeRef(os, token);
  case tabidField:      return Field(os, token);
  case tabidMethodDef:  return MethodDef_Def(os, token);
  case tabidMemberRef:  return MemberRef(os, token);
  case tabidString:     return UserString(os, token.Index());
  default:
    break;
  }
  return os;
}