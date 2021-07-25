#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <memory>

class EZ_RHI_DLL Shader : public ezRefCounted
{
public:
  virtual ~Shader() = default;
  virtual ShaderType GetType() const = 0;
  virtual const ezDynamicArray<ezUInt8>& GetBlob() const = 0;
  virtual ezUInt64 GetId(const ezString& entry_point) const = 0;
  virtual const BindKey& GetBindKey(const ezString& name) const = 0;
  virtual const ezDynamicArray<ResourceBindingDesc>& GetResourceBindings() const = 0;
  virtual const ResourceBindingDesc& GetResourceBinding(const BindKey& bind_key) const = 0;
  virtual const ezDynamicArray<InputLayoutDesc>& GetInputLayouts() const = 0;
  virtual ezUInt32 GetInputLayoutLocation(const ezString& semantic_name) const = 0;
  virtual const ezDynamicArray<BindKey>& GetBindings() const = 0;
  virtual const ezSharedPtr<ShaderReflection>& GetReflection() const = 0;
};
