#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <memory>
#include <string>
#include <vector>

enum class ShaderKind
{
  kUnknown = 0,
  kPixel,
  kVertex,
  kGeometry,
  kCompute,
  kLibrary,
  kRayGeneration,
  kIntersection,
  kAnyHit,
  kClosestHit,
  kMiss,
  kCallable,
  kMesh,
  kAmplification,
};

struct EZ_RHI_DLL EntryPoint
{
  EZ_DECLARE_POD_TYPE();
  ezString name;
  ShaderKind kind;
  ezUInt32 payload_size;
  ezUInt32 attribute_size;
};

inline bool operator==(const EntryPoint& lhs, const EntryPoint& rhs)
{
  return std::tie(lhs.name, lhs.kind) == std::tie(rhs.name, rhs.kind);
}

inline bool operator<(const EntryPoint& lhs, const EntryPoint& rhs)
{
  return std::tie(lhs.name, lhs.kind) < std::tie(rhs.name, rhs.kind);
}

inline auto MakeTie(const ResourceBindingDesc& desc)
{
  return std::tie(desc.name, desc.type, desc.slot, desc.space, desc.dimension);
};

inline bool operator==(const ResourceBindingDesc& lhs, const ResourceBindingDesc& rhs)
{
  return MakeTie(lhs) == MakeTie(rhs);
}

inline bool operator<(const ResourceBindingDesc& lhs, const ResourceBindingDesc& rhs)
{
  return MakeTie(lhs) < MakeTie(rhs);
}

struct EZ_RHI_DLL InputParameterDesc
{
  EZ_DECLARE_POD_TYPE();
  ezUInt32 location;
  ezString semantic_name;
  ezRHIResourceFormat::Enum format;
};

struct EZ_RHI_DLL OutputParameterDesc
{
  EZ_DECLARE_POD_TYPE();
  ezUInt32 slot;
};

enum class VariableType
{
  kStruct,
  kFloat,
  kInt,
  kUint,
  kBool,
};

struct EZ_RHI_DLL VariableLayout
{
  EZ_DECLARE_POD_TYPE();
  ezString name;
  VariableType type;
  ezUInt32 offset;
  ezUInt32 size;
  ezUInt32 rows;
  ezUInt32 columns;
  ezUInt32 elements;
  ezDynamicArray<VariableLayout> members;
};

inline auto MakeTie(const VariableLayout& desc)
{
  return std::tie(desc.name, desc.type, desc.offset, desc.size, desc.rows, desc.columns, desc.elements, desc.members);
};

inline bool operator==(const VariableLayout& lhs, const VariableLayout& rhs)
{
  return MakeTie(lhs) == MakeTie(rhs);
}

inline bool operator<(const VariableLayout& lhs, const VariableLayout& rhs)
{
  return MakeTie(lhs) < MakeTie(rhs);
}

inline bool operator==(const ezDynamicArray<VariableLayout>& lhs, const ezDynamicArray<VariableLayout>& rhs) {
  if (lhs.GetCount() != rhs.GetCount())
  {
    return false;
  }

  for (ezUInt32 i = 0; i < lhs.GetCount(); i++)
  {
    if (!(lhs[i].members == rhs[i].members))
    {
      return false;
    }
  }

  return true;
}

inline bool operator<(const ezDynamicArray<VariableLayout>& lhs, const ezDynamicArray<VariableLayout>& rhs)
{
  if ((lhs == rhs))
  {
    return false;
  }

  return lhs.GetCount() < rhs.GetCount();
}

struct EZ_RHI_DLL ShaderFeatureInfo
{
  bool resource_descriptor_heap_indexing = false;
  bool sampler_descriptor_heap_indexing = false;
};

class EZ_RHI_DLL ShaderReflection : public ezRefCounted
{
public:
  virtual ~ShaderReflection() = default;
  virtual const ezDynamicArray<EntryPoint>& GetEntryPoints() const = 0;
  virtual const ezDynamicArray<ResourceBindingDesc>& GetBindings() const = 0;
  virtual const ezDynamicArray<VariableLayout>& GetVariableLayouts() const = 0;
  virtual const ezDynamicArray<InputParameterDesc>& GetInputParameters() const = 0;
  virtual const ezDynamicArray<OutputParameterDesc>& GetOutputParameters() const = 0;
  virtual const ShaderFeatureInfo& GetShaderFeatureInfo() const = 0;
};
