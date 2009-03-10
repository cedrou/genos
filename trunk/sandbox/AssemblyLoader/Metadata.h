#pragma once

#include "../../src/kernel/common.h"
#include <iostream>
#include <vector>

#pragma warning (disable:4200) // nonstandard extension used : zero-sized array in struct/union

namespace GenOS
{
  class MdPhysicalLayout;

  enum MdTableId
  {
    tabidFirst                   = 0x00,

    tabidModule                  = tabidFirst,
    tabidTypeRef                 = 0x01,
    tabidTypeDef                 = 0x02,
                              // = 0x03,
    tabidField                   = 0x04,
                              // = 0x05,
    tabidMethodDef               = 0x06,
                              // = 0x07,
    tabidParam                   = 0x08,
    tabidInterfaceImpl           = 0x09,
    tabidMemberRef               = 0x0A,
    tabidConstant                = 0x0B,
    tabidCustomAttribute         = 0x0C,
    tabidFieldMarshal            = 0x0D,
    tabidDeclSecurity            = 0x0E,
    tabidClassLayout             = 0x0F,

    tabidFieldLayout             = 0x10,
    tabidStandAloneSig           = 0x11,
    tabidEventMap                = 0x12,
                              // = 0x13,
    tabidEvent                   = 0x14,
    tabidPropertyMap             = 0x15,
                              // = 0x16,
    tabidProperty                = 0x17,
    tabidMethodSemantics         = 0x18,
    tabidMethodImpl              = 0x19,
    tabidModuleRef               = 0x1A,
    tabidTypeSpec                = 0x1B,
    tabidImplMap                 = 0x1C,
    tabidFieldRVA                = 0x1D,
                              // = 0x1E,
                              // = 0x1F,

    tabidAssembly                = 0x20,
    tabidAssemblyProcessor       = 0x21,
    tabidAssemblyOS              = 0x22,
    tabidAssemblyRef             = 0x23,
    tabidAssemblyRefProcessor    = 0x24,
    tabidAssemblyRefOS           = 0x25,
    tabidFile                    = 0x26,
    tabidExportedType            = 0x27,
    tabidManifestResource        = 0x28,
    tabidNestedClass             = 0x29,
    tabidGenericParam            = 0x2A,
    tabidMethodSpec              = 0x2B,
    tabidGenericParamConstraint  = 0x2C,

    tabidCount,

    tabidString                  = 0x70
  };

  enum MdRecordItemType
  {
    recItemVOID                = 0x00,

    // Constant
    recItemConstant            = 0x00,
    recItemUINT8               = 0x01,            // 1-byte value
    recItemUINT16              = 0x02,           // 2-bytes value
    recItemUINT32              = 0x04,           // 4-bytes value

    // Heap Index
    recItemHeapIndex           = 0x10,
    recItemSTRING              = 0x11,
    recItemGUID                = 0x12,
    recItemBLOB                = 0x14,

    // Table Index
    recItemTableIndex          = 0x20,
    recItemTYPEDEF             = 0x22,
    recItemFIELD               = 0x24,
    recItemMETHODDEF           = 0x26,
    recItemPARAM               = 0x28,
    recItemEVENT               = 0x34,
    recItemPROPERTY            = 0x37,
    recItemMODULEREF           = 0x3A,
    recItemASSEMBLYREF         = 0x43,
    recItemGENERICPARAM        = 0x4A,

    // Coded Index
    recItemCodedIndex          = 0x50,
    recItemTYPEDEFORREF        = 0x51,     // TypeDef, TypeRef or TypeSpec
    recItemHASCONSTANT         = 0x52,     // Field, Param or Property
    recItemHASCUSTOMATTRIBUTE  = 0x53,     // 
    recItemHASFIELDMARSHALL    = 0x54,     // Field, Param
    recItemHASDECLSECURITY     = 0x55,     // 
    recItemMEMBERREFPARENT     = 0x56,     // 
    recItemHASSEMANTICS        = 0x57,     // 
    recItemMETHODDEFORREF      = 0x58,     //
    recItemMEMBERFORWARDED     = 0x59,     //
    recItemIMPLEMENTATION      = 0x5A,     //
    recItemCUSTOMATTRIBUTETYPE = 0x5B,     //
    recItemRESOLUTIONSCOPE     = 0x5C,     // Module, ModuleRef, AssemblyRef or TypeRef
    recItemTYPEORMETHODDEF     = 0x5D,     //

    recItemCount
  };

  struct CodedIndexFormat
  {
    const MdTableId*  Tags;
    uint8             Size;
    uint8             Bits;
  };

  struct CodedIndex
  {
    MdTableId tabid;
    uint32    index;
  };

  class MdRecordItem
  {
  public:
    MdRecordItemType  Type;
    uint32            Value;

    MdRecordItem(MdRecordItemType type, const uint8* data, MdPhysicalLayout* mdpl);
  };

  class MdRecord
  {
  private:
    const uint8*            _data;
    const MdRecordItemType* _schema;
    const uint8*            _offsets;
    uint32                  _count;

  public:
    MdRecord(const uint8* data, const MdRecordItemType* schema, const uint8* offset);

    MdRecordItem  GetItem(uint32 item, MdPhysicalLayout* mdpl) const;
    uint32        Count() const { return _count; }
  };

  class MdCodedIndex
  {
  private:
    static CodedIndexFormat _codedIndexTags[]; 

  public:
    //MdCodedIndex(MdRecordItemType type, uint32 codix);

    static CodedIndex Decode(const MdRecordItem& item);
    static CodedIndex Decode(MdRecordItemType type, uint32 codix);

    friend class MdTable;
    friend class MdToken;
  };

  class MdTable
  {
  private:
    static const MdRecordItemType* _schemas[tabidCount];

    // Cache of calculated item sizes
    static uint8 _itemSizes[recItemCount];

  public:
    static const char* Names[tabidCount];

  private:
    MdTableId     _tabid;

    uint32        _recordSize;
    uint32        _count;
    const uint8*  _firstRecord;

    // Cache of calculated item offsets inside a record
    uint8         _offsets[9];

  public:
    MdTable();
    void InitializeStep1(MdTableId tabid, uint32 count);
    void InitializeStep2(const uint8* start, MdPhysicalLayout* mdpl);
    
    uint32  RecordSize() const { return _recordSize; }
    uint32  RecordCount() const { return _count; }
    MdRecord GetRecord(uint32 index) const;

    static uint32 GetRecordItemSize(MdRecordItemType type, MdPhysicalLayout* mdpl);
    static uint32 GetRecordSize(MdTableId tabid, MdPhysicalLayout* mdpl);
    static void DumpTables(MdPhysicalLayout* mdpl);

    void Dump(MdPhysicalLayout* mdpl);
  };

  class MdToken
  {
  private:
    uint32 _token;

  public:
    MdToken(uint32 token);
    MdToken(MdTableId tabid, uint32 index);
    MdToken(MdRecordItemType type, uint32 codix);
    MdToken(const MdRecordItem& item);

    MdTableId TabID() const;
    uint32    Index() const;
    uint32    Token() const;

    MdToken   Max(const MdTable* tables) const;

    MdToken operator+(uint32 i) const;
    MdToken operator-(uint32 i) const;
    MdToken& operator++();
    bool operator==(const MdToken& tk) const;
    bool operator!=(const MdToken& tk) const;
    bool operator>(const MdToken& tk) const;
  };

  enum CilArgType
  {
    cilNONE,
    cilUINT8,
    cilUINT16,
    cilUINT32,
    cilR4,
    cilR8,
    cilTOKEN,
    cilOFFSET8,
    cilOFFSET32,
  };

  class CilOpCode
  {
  public:
    uint8             opcode;
    const CilOpCode*  next;
    const char*       name;
    bool              prefix;
    uint8             codebytes;   
    CilArgType        argtype;

  public:
    static const CilOpCode* Find(const uint8* cil);
  };

  enum MdStreamType
  {
    streamString,
    streamUS,
    streamBlob,
    streamGuid,
    streamTable,
    streamNonOptTable,

    streamCount
  };

                            
                            
#define METADATA_MAGIC      0x424A5342  // BSJB
#define STREAM_NAME_MAXLEN  32

  class MdPhysicalLayout
  {
  private:
    struct MdRoot
    {
      uint32  Signature;        // "Magic" signature.
      uint16  MajorVersion;     // Major file version.
      uint16  MinorVersion;     // Minor file version.
      uint32  ExtraData;        // Offset to next structure of information 
      uint32  VersionLength;    // Length of version string
      uint8   VersionString[0]; // Version string
    };

    struct MdRoot2
    {
      uint8   Flags;
      uint8   pad;
      uint16  NbStreams;
    };

    struct MdStreamHeader
    {
      uint32  Offset;   // Offset in file for this stream.
      uint32  Size;     // Size of the file.
    };

    struct MdTableHeader
    {
      uint32  res1;         // Reserved, always 0
      uint8   MajorVersion; // Major version of table schema; shall be 2
      uint8   MinorVersion; // Minor version of table schema; shall be 0
      uint8   HeapSizes;    // Bit vector for heap sizes
      uint8   res2;         // Reserved, always 1 
      uint64  Valid;        // Bit vector of present tables, let n be the number of bits that are 1.
      uint64  Sorted;       // Bit vector of sorted tables.
      uint32  Rows[0];      // Array of n 4-byte unsigned integers indicating the number of rows for each present table.
    };

  private:
    // Several headers in the PE file
    void*           dos;
    void*           nt32;
    void*           sections;
    void*           net;

    MdRoot*         mdRoot;
    MdRoot2*        mdHeader;
    MdStreamHeader  mdStreams[streamCount];
    MdTableHeader*  mdTableHeader;

    // Metadata tables
    MdTable         mdTables[tabidCount];

  private:
    MdPhysicalLayout();

  public:
    static MdPhysicalLayout* Initialize(const uint8* data);

    const uint8*  GetString(uint32 index) const;
    const uint8*  GetUserString(uint32 index) const;
    const uint8*  GetBlob(uint32 index) const;
    const uint8*  GetGuid(uint32 index) const;
    const uint8*  GetMethod(uint32 rva) const;

    uint32        GetImageBase() const;
    uint32        GetFileAlignment() const;
    uint32        GetStackReserve() const;
    uint32        GetSubsystem() const;
    uint32        GetCorFlags() const;
    MdToken       GetEntryPoint() const;

    MdRecord      GetRecord(const MdToken& token) const;
    MdRecord      GetRecord(MdTableId tabid, uint32 index) const;
    uint32        NbRecords(MdTableId tabid) const;

    MdToken       GetMaxToken(MdTableId tabid) const;

  private:
    const uint8*  GetStream(MdStreamType type) const;

    friend class MdTable;
    friend class MdDisassembler;
  };

  class MdDisassembler
  {
  private:
    MdPhysicalLayout* _mdpl;

  public :
    MdDisassembler(MdPhysicalLayout* _mdpl);

    void Disassemble(std::ostream& os) const;

  private:
    void ExternModules(std::ostream& os) const;
    void ExternAssemblies(std::ostream& os) const;
    void Assembly(std::ostream& os) const;
    void Module(std::ostream& os) const;
    void Classes(std::ostream& os) const;


    std::ostream& Hex  (std::ostream& os, uint32 value) const;
    std::ostream& Hex8 (std::ostream& os, uint32 value) const;
    std::ostream& Hex16(std::ostream& os, uint32 value) const;
    std::ostream& Hex32(std::ostream& os, uint32 value) const;
    std::ostream& hex  (std::ostream& os, uint32 value) const;
    std::ostream& hex8 (std::ostream& os, uint32 value) const;
    std::ostream& hex16(std::ostream& os, uint32 value) const;
    std::ostream& hex32(std::ostream& os, uint32 value) const;

    std::ostream& String    (std::ostream& os, const MdRecordItem& item) const;
    std::ostream& UserString(std::ostream& os, uint32 index) const;
    std::ostream& Blob      (std::ostream& os, const MdRecordItem& item) const;
    std::ostream& Guid      (std::ostream& os, const MdRecordItem& item) const;

    std::ostream& SignatureElementType(std::ostream& os, const uint8*& signature) const;
    std::ostream& CustomAttribute(std::ostream& os, const MdRecord& record) const;

    std::ostream& Token         (std::ostream& os, const MdToken& token) const;

    std::ostream& TypeDef       (std::ostream& os, const MdToken& token) const;
    std::ostream& TypeDef_Impl  (std::ostream& os, const MdToken& token) const;
    std::ostream& TypeRef       (std::ostream& os, const MdToken& token) const;
    std::ostream& TypeSpec      (std::ostream& os, const MdToken& token) const;
    std::ostream& MemberRef     (std::ostream& os, const MdToken& token) const;
    std::ostream& MethodDef     (std::ostream& os, const MdToken& token) const;
    std::ostream& MethodDef_Impl(std::ostream& os, const MdToken& token) const;
    std::ostream& Field         (std::ostream& os, const MdToken& token) const;
    std::ostream& Field_Impl    (std::ostream& os, const MdToken& token) const;

    std::ostream& TypeDefOrRef  (std::ostream& os, const MdToken& token) const;

    std::vector<MdRecord> FindCustomAttributes(const MdToken& tk) const;
    MdRecord      FindConstant(const MdToken& tk) const;
    MdRecord      FindPInvokeImpl(const MdToken& tk) const;
    MdToken       FindTypeDef(const MdToken& tk) const;

    uint32        DecodeSignatureItem(const uint8*& signature) const;

  };
}