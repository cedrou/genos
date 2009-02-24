#include "Metadata.h"

using namespace GenOS;


static const MdTableId tagsTypeDefOrRef[] = { 
  tabidTypeDef, 
  tabidTypeRef, 
  tabidTypeSpec 
};
static const MdTableId tagsHasConstant[] = { 
  tabidField, 
  tabidParam, 
  tabidProperty 
};
static const MdTableId tagsHasCustomAttribute[] = { 
  tabidMethodDef,
  tabidField,
  tabidTypeRef,
  tabidTypeDef,
  tabidParam,
  tabidInterfaceImpl,
  tabidMemberRef,
  tabidModule,
  tabidDeclSecurity, //Permission,
  tabidProperty,
  tabidEvent,
  tabidStandAloneSig,
  tabidModuleRef,
  tabidTypeSpec,
  tabidAssembly,
  tabidAssemblyRef,
  tabidFile,
  tabidExportedType,
  tabidManifestResource 
};
static const MdTableId tagsHasFieldMarshall[] = { 
  tabidField,
  tabidParam,
};
static const MdTableId tagsHasDeclSecurity[] = { 
  tabidTypeDef,
  tabidMethodDef,
  tabidAssembly
};
static const MdTableId tagsMemberRefParent[] = { 
  tabidTypeDef,
  tabidTypeRef,
  tabidModuleRef,
  tabidMethodDef,
  tabidTypeSpec
};
static const MdTableId tagsHasSemantics[] = { 
  tabidEvent,
  tabidProperty
};
static const MdTableId tagsMethodDefOrRef[] = { 
  tabidMethodDef,
  tabidMemberRef
};
static const MdTableId tagsMemberForwarded[] = { 
  tabidField,
  tabidMethodDef
};
static const MdTableId tagsImplementation[] = { 
  tabidFile,
  tabidAssemblyRef,
  tabidExportedType
};
static const MdTableId tagsCustomAttributeType[] = { 
  tabidCount,
  tabidCount,
  tabidMethodDef,
  tabidMemberRef,
  tabidCount
};
static const MdTableId tagsResolutionScope[] = { 
  tabidModule,
  tabidModuleRef,
  tabidAssemblyRef,
  tabidTypeRef
};
static const MdTableId tagsTypeOrMethodDef[] = { 
  tabidTypeDef,
  tabidMethodDef
};

CodedIndexFormat MdCodedIndex::_codedIndexTags[] = 
{
  { NULL, 0, 0 },
  { tagsTypeDefOrRef,        3, 2 },
  { tagsHasConstant,         3, 2 },
  { tagsHasCustomAttribute, 19, 5 },
  { tagsHasFieldMarshall,    2, 1 },
  { tagsHasDeclSecurity,     3, 2 },
  { tagsMemberRefParent,     5, 3 },
  { tagsHasSemantics,        2, 1 },
  { tagsMethodDefOrRef,      2, 1 },
  { tagsMemberForwarded,     2, 1 },
  { tagsImplementation,      3, 2 },
  { tagsCustomAttributeType, 5, 3 },
  { tagsResolutionScope,     4, 2 },
  { tagsTypeOrMethodDef,     2, 1 },
};


//MdCodedIndex::MdCodedIndex(MdRecordItemType type, uint32 codix)
//{
//}

CodedIndex MdCodedIndex::Decode(MdRecordItemType type, uint32 codix)
{
  const CodedIndexFormat& fmt = _codedIndexTags[type - recItemCodedIndex];
  uint32 tableindex = codix & ((1 << fmt.Bits) - 1);
  CodedIndex ci;
  ci.tabid = fmt.Tags[tableindex];
  ci.index = codix >> fmt.Bits;
  return ci;
}

CodedIndex MdCodedIndex::Decode(const MdRecordItem& item)
{
  return Decode(item.Type, item.Value);
}
