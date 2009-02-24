#include "Metadata.h"

using namespace GenOS;


//=============================================
// SCHEMAS
//=============================================

const MdRecordItemType schModule[] = {
  recItemUINT16,              // Generation 
  recItemSTRING,              // Name
  recItemGUID,                // MvId
  recItemGUID,                // EncId
  recItemGUID,                // EncBaseId
  recItemVOID
};
const MdRecordItemType schTypeRef[] = {
  recItemRESOLUTIONSCOPE, 
  recItemSTRING,              // TypeName 
  recItemSTRING,              // TypeNamespace 
  recItemVOID
};
const MdRecordItemType schTypeDef[] = { 
  recItemUINT32,              // Flags
  recItemSTRING,              // TypeName
  recItemSTRING,              // TypeNamespace 
  recItemTYPEDEFORREF,        // Extends
  recItemFIELD,               // FieldList
  recItemMETHODDEF,           // MethodList
  recItemVOID
};
const MdRecordItemType schField[] = { 
  recItemUINT16,              // Flags (FieldAttribute)
  recItemSTRING,              // Name
  recItemBLOB,                // Signature
  recItemVOID
};
const MdRecordItemType schMethodDef[] = { 
  recItemUINT32,              // RVA
  recItemUINT16,              // ImplFlags (MethodImplAttributes)
  recItemUINT16,              // Flags (MethodAttributes)
  recItemSTRING,              // Name
  recItemBLOB,                // Signature
  recItemPARAM,               // ParamList
  recItemVOID
};
const MdRecordItemType schParam[] = { 
  recItemUINT16,              // Flags (ParamAttributes)
  recItemUINT16,              // Sequence
  recItemSTRING,              // Name
  recItemVOID
};
const MdRecordItemType schInterfaceImpl[] = { 
  recItemTYPEDEF,             // Class
  recItemTYPEDEFORREF,        // Interface
  recItemVOID
};
const MdRecordItemType schMemberRef[] = { 
  recItemMEMBERREFPARENT,     // Class
  recItemSTRING,              // Name
  recItemBLOB,                // Signature
  recItemVOID
};
const MdRecordItemType schConstant[] = { 
  recItemUINT8,               // Type
  recItemUINT8,               // Padding
  recItemHASCONSTANT,         // Parent
  recItemBLOB,                // Value
  recItemVOID
};
const MdRecordItemType schCustomAttribute[] = { 
  recItemHASCUSTOMATTRIBUTE,  // Parent
  recItemCUSTOMATTRIBUTETYPE, // Type
  recItemBLOB,                // Value
  recItemVOID
};
const MdRecordItemType schFieldMarshal[] = { 
  recItemHASFIELDMARSHALL,    // Parent
  recItemBLOB,                // NativeType
  recItemVOID
};
const MdRecordItemType schDeclSecurity[] = { 
  recItemUINT16,              // Action
  recItemHASDECLSECURITY,     // Parent
  recItemBLOB,                // PermissionSet
  recItemVOID
};
const MdRecordItemType schClassLayout[] = { 
  recItemUINT16,              // PackingSize
  recItemUINT32,              // ClassSize
  recItemTYPEDEF,             // Parent
  recItemVOID
};
const MdRecordItemType schFieldLayout[] = { 
  recItemUINT32,              // Offset
  recItemFIELD,               // Field
  recItemVOID
};
const MdRecordItemType schStandAloneSig[] = { 
  recItemBLOB,                // Signature
  recItemVOID
};
const MdRecordItemType schEventMap[] = { 
  recItemTYPEDEF,             // Parent
  recItemEVENT,               // EventList
  recItemVOID
};
const MdRecordItemType schEvent[] = { 
  recItemUINT16,              // EventFlags (EventAttributes)
  recItemSTRING,              // Name
  recItemTYPEDEFORREF,        // EventType
  recItemVOID
};
const MdRecordItemType schPropertyMap[] = { 
  recItemTYPEDEF,             // Parent
  recItemPROPERTY,            // PropertyList
  recItemVOID
};
const MdRecordItemType schProperty[] = { 
  recItemUINT16,              // Flags (PropertyAttributes)
  recItemSTRING,              // Name
  recItemBLOB,                // Type
  recItemVOID
};
const MdRecordItemType schMethodSemantics[] = { 
  recItemUINT16,              // Semantics (MethodSemanticsAttributes)
  recItemMETHODDEF,           // Method
  recItemHASSEMANTICS,        // Association
  recItemVOID
};
const MdRecordItemType schMethodImpl[] = { 
  recItemTYPEDEF,             // Class
  recItemMETHODDEFORREF,      // MethodBody
  recItemMETHODDEFORREF,      // MethodDeclaration
  recItemVOID
};
const MdRecordItemType schModuleRef[] = { 
  recItemSTRING,              // Name
  recItemVOID
};
const MdRecordItemType schTypeSpec[] = { 
  recItemBLOB,                // Signature
  recItemVOID
};
const MdRecordItemType schImplMap[] = { 
  recItemUINT16,              // MappingFlags (PInvokeAttributes)
  recItemMEMBERFORWARDED,     // MemberForwarded
  recItemSTRING,              // ImportName
  recItemMODULEREF,           // ImportScope
  recItemVOID
};
const MdRecordItemType schFieldRVA[] = { 
  recItemUINT32,              // RVA
  recItemFIELD,               // Field
  recItemVOID
};
const MdRecordItemType schAssembly[] = { 
  recItemUINT32,              // HashAlgId (AssemblyHashAlgorithm)
  recItemUINT16,              // MajorVersion
  recItemUINT16,              // MinorVersion
  recItemUINT16,              // BuildNumber
  recItemUINT16,              // RevisionNumber
  recItemUINT32,              // Flags (AssemblyFlags)
  recItemBLOB,                // PublicKey
  recItemSTRING,              // Name
  recItemSTRING,              // Culture
  recItemVOID
};
const MdRecordItemType schAssemblyProcessor[] = { 
  recItemUINT32,              // Processor
  recItemVOID
};
const MdRecordItemType schAssemblyOS[] = { 
  recItemUINT32,              // OSPlatformID
  recItemUINT32,              // OSMajorVersion
  recItemUINT32,              // OSMinorVersion
  recItemVOID
};
const MdRecordItemType schAssemblyRef[] = { 
  recItemUINT16,              // MajorVersion
  recItemUINT16,              // MinorVersion
  recItemUINT16,              // BuildNumber
  recItemUINT16,              // RevisionNumber
  recItemUINT32,              // Flags (AssemblyFlags)
  recItemBLOB,                // PublicKeyOrToken
  recItemSTRING,              // Name
  recItemSTRING,              // Culture
  recItemBLOB,                // HashValue
  recItemVOID
};
const MdRecordItemType schAssemblyRefProcessor[] = { 
  recItemUINT32,              // Processor
  recItemASSEMBLYREF,         // AssemblyRef
  recItemVOID
};
const MdRecordItemType schAssemblyRefOS[] = { 
  recItemUINT32,              // OSPlatformID
  recItemUINT32,              // OSMajorVersion
  recItemUINT32,              // OSMinorVersion
  recItemASSEMBLYREF,         // AssemblyRef
  recItemVOID
};
const MdRecordItemType schFile[] = { 
  recItemUINT32,              // Flags (FileAttributes)
  recItemSTRING,              // Name
  recItemBLOB,                // HashValue
  recItemVOID
};
const MdRecordItemType schExportedType[] = { 
  recItemUINT32,              // Flags (TypeAttributes)
  recItemTYPEDEF,             // TypeDefId
  recItemSTRING,              // TypeName
  recItemSTRING,              // TypeNamespace
  recItemIMPLEMENTATION,      // Implementation
  recItemVOID
};
const MdRecordItemType schManifestResource[] = { 
  recItemUINT32,              // Offset
  recItemUINT32,              // Flags (ManifestResourceAttributes)
  recItemSTRING,              // Name
  recItemIMPLEMENTATION,      // Implementation
  recItemVOID
};
const MdRecordItemType schNestedClass[] = { 
  recItemTYPEDEF,             // NestedClass
  recItemTYPEDEF,             // EnclosingClass
  recItemVOID
};
const MdRecordItemType schGenericParam[] = { 
  recItemUINT16,              // Number
  recItemUINT16,              // Flags (GenericParamAttributes)
  recItemTYPEORMETHODDEF,     // Owner
  recItemSTRING,              // Name
  recItemVOID
};
const MdRecordItemType schMethodSpec[] = { 
  recItemMETHODDEFORREF,      // Method
  recItemBLOB,                // Instantiation
  recItemVOID
};
const MdRecordItemType schGenericParamConstraint[] = { 
  recItemGENERICPARAM,        // Owner
  recItemTYPEDEFORREF,        // Constraint
  recItemVOID
};
const MdRecordItemType schVoid[] = { 
  recItemVOID
};

const MdRecordItemType* MdTable::_schemas[tabidCount] = 
{
  schModule,
  schTypeRef,
  schTypeDef,
  schVoid,
  schField,
  schVoid,
  schMethodDef,
  schVoid,
  schParam,
  schInterfaceImpl,
  schMemberRef,
  schConstant,
  schCustomAttribute,
  schFieldMarshal,
  schDeclSecurity,
  schClassLayout,

  schFieldLayout,
  schStandAloneSig,
  schEventMap,
  schVoid,
  schEvent,
  schPropertyMap,
  schVoid,
  schProperty,
  schMethodSemantics,
  schMethodImpl,
  schModuleRef,
  schTypeSpec,
  schImplMap,
  schFieldRVA,
  schVoid,
  schVoid,

  schAssembly,
  schAssemblyProcessor,
  schAssemblyOS,
  schAssemblyRef,
  schAssemblyRefProcessor,
  schAssemblyRefOS,
  schFile,
  schExportedType,
  schManifestResource,
  schNestedClass,
  schGenericParam,
  schMethodSpec,
  schGenericParamConstraint
};

