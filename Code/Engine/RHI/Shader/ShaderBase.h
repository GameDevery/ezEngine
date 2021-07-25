#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Shader/Shader.h>
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <map>

class EZ_RHI_DLL ShaderBase : public Shader
{
public:
  ShaderBase(const ShaderDesc& desc, ezDynamicArray<ezUInt8> byteCode, ezSharedPtr<ShaderReflection> reflection, ShaderBlobType blob_type);
  ShaderType GetType() const override;
  const ezDynamicArray<ezUInt8>& GetBlob() const override;
  ezUInt64 GetId(const ezString& entry_point) const override;
  const BindKey& GetBindKey(const ezString& name) const override;
  const ezDynamicArray<ResourceBindingDesc>& GetResourceBindings() const override;
  const ResourceBindingDesc& GetResourceBinding(const BindKey& bind_key) const override;
  const ezDynamicArray<InputLayoutDesc>& GetInputLayouts() const override;
  ezUInt32 GetInputLayoutLocation(const ezString& semantic_name) const override;
  const ezDynamicArray<BindKey>& GetBindings() const override;
  const ezSharedPtr<ShaderReflection>& GetReflection() const override;

protected:
  ShaderType m_shader_type;
  ShaderBlobType m_blob_type;
  ezDynamicArray<ezUInt8> m_blob;
  std::map<ezString, ezUInt64> m_ids;
  ezDynamicArray<ResourceBindingDesc> m_bindings;
  ezDynamicArray<BindKey> m_binding_keys;
  std::map<BindKey, ezUInt32> m_mapping;
  std::map<ezString, BindKey> m_bind_keys;
  ezDynamicArray<InputLayoutDesc> m_input_layout_descs;
  std::map<ezString, ezUInt32> m_locations;
  ezSharedPtr<ShaderReflection> m_reflection;
};
