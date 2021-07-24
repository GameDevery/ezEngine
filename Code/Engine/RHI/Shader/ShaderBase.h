#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Shader/Shader.h>
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <map>

class EZ_RHI_DLL ShaderBase : public Shader
{
public:
  ShaderBase(const ShaderDesc& desc, std::vector<ezUInt8> byteCode, std::shared_ptr<ShaderReflection> reflection, ShaderBlobType blob_type);
  ShaderType GetType() const override;
  const std::vector<ezUInt8>& GetBlob() const override;
  ezUInt64 GetId(const ezString& entry_point) const override;
  const BindKey& GetBindKey(const ezString& name) const override;
  const std::vector<ResourceBindingDesc>& GetResourceBindings() const override;
  const ResourceBindingDesc& GetResourceBinding(const BindKey& bind_key) const override;
  const std::vector<InputLayoutDesc>& GetInputLayouts() const override;
  ezUInt32 GetInputLayoutLocation(const ezString& semantic_name) const override;
  const std::vector<BindKey>& GetBindings() const override;
  const std::shared_ptr<ShaderReflection>& GetReflection() const override;

protected:
  ShaderType m_shader_type;
  ShaderBlobType m_blob_type;
  std::vector<ezUInt8> m_blob;
  std::map<ezString, ezUInt64> m_ids;
  std::vector<ResourceBindingDesc> m_bindings;
  std::vector<BindKey> m_binding_keys;
  std::map<BindKey, size_t> m_mapping;
  std::map<ezString, BindKey> m_bind_keys;
  std::vector<InputLayoutDesc> m_input_layout_descs;
  std::map<ezString, ezUInt32> m_locations;
  std::shared_ptr<ShaderReflection> m_reflection;
};
