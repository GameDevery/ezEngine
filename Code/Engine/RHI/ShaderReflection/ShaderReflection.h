#pragma once
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/RHIDLL.h>
#include <RHI/Resource/Resource.h>
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

struct EntryPoint
{
  std::string name;
  ShaderKind kind;
  uint32_t payload_size;
  uint32_t attribute_size;
};

//inline bool operator==(const EntryPoint& lhs, const EntryPoint& rhs)
//{
//  return std::tie(lhs.name, lhs.kind) == std::tie(rhs.name, rhs.kind);
//}
//
//inline bool operator<(const EntryPoint& lhs, const EntryPoint& rhs)
//{
//  return std::tie(lhs.name, lhs.kind) < std::tie(rhs.name, rhs.kind);
//}

//inline auto MakeTie(const ResourceBindingDesc& desc)
//{
//  return std::tie(desc.name, desc.type, desc.slot, desc.space, desc.dimension);
//};

//inline bool operator==(const ResourceBindingDesc& lhs, const ResourceBindingDesc& rhs)
//{
//  return MakeTie(lhs) == MakeTie(rhs);
//}
//
//inline bool operator<(const ResourceBindingDesc& lhs, const ResourceBindingDesc& rhs)
//{
//  return MakeTie(lhs) < MakeTie(rhs);
//}

struct InputParameterDesc
{
  uint32_t location;
  std::string semantic_name;
  ezRHIResourceFormat::Enum format;
};

struct OutputParameterDesc
{
  uint32_t slot;
};

enum class VariableType
{
  kStruct,
  kFloat,
  kInt,
  kUint,
  kBool,
};

struct VariableLayout
{
  std::string name;
  VariableType type;
  uint32_t offset;
  uint32_t size;
  uint32_t rows;
  uint32_t columns;
  uint32_t elements;
  std::vector<VariableLayout> members;
};

//inline auto MakeTie(const VariableLayout& desc)
//{
//  return std::tie(desc.name, desc.type, desc.offset, desc.size, desc.rows, desc.columns, desc.elements, desc.members);
//};

//inline bool operator==(const VariableLayout& lhs, const VariableLayout& rhs)
//{
//  return MakeTie(lhs) == MakeTie(rhs);
//}
//
//inline bool operator<(const VariableLayout& lhs, const VariableLayout& rhs)
//{
//  return MakeTie(lhs) < MakeTie(rhs);
//}

struct ShaderFeatureInfo
{
  bool resource_descriptor_heap_indexing = false;
  bool sampler_descriptor_heap_indexing = false;
};

class ShaderReflection : public QueryInterface
{
public:
  virtual ~ShaderReflection() = default;
  virtual const std::vector<EntryPoint>& GetEntryPoints() const = 0;
  virtual const std::vector<ResourceBindingDesc>& GetBindings() const = 0;
  virtual const std::vector<VariableLayout>& GetVariableLayouts() const = 0;
  virtual const std::vector<InputParameterDesc>& GetInputParameters() const = 0;
  virtual const std::vector<OutputParameterDesc>& GetOutputParameters() const = 0;
  virtual const ShaderFeatureInfo& GetShaderFeatureInfo() const = 0;
};
