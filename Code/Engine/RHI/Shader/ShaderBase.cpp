#include <RHI/Shader/ShaderBase.h>

static ezUInt64 GenId()
{
  static ezUInt64 id = 0;
  return ++id;
}

ShaderBase::ShaderBase(const ShaderDesc& desc, ezDynamicArray<ezUInt8> byteCode, ezSharedPtr<ShaderReflection> reflection, ShaderBlobType blob_type)
  : m_shader_type(desc.type)
  , m_blob_type(blob_type)
{
  m_blob = byteCode;
  m_reflection = reflection;
  m_bindings = m_reflection->GetBindings();
  for (ezUInt32 i = 0; i < m_bindings.GetCount(); ++i)
  {
    BindKey bind_key = {m_shader_type, m_bindings[i].type, m_bindings[i].slot, m_bindings[i].space, m_bindings[i].count};
    m_bind_keys[m_bindings[i].name] = bind_key;
    m_mapping[bind_key] = i;
    m_binding_keys.PushBack(bind_key);
  }

  decltype(auto) input_parameters = m_reflection->GetInputParameters();
  for (ezUInt32 i = 0; i < input_parameters.GetCount(); ++i)
  {
    decltype(auto) layout = m_input_layout_descs.ExpandAndGetRef();
    layout.slot = i;
    layout.semantic_name = input_parameters[i].semantic_name;
    layout.format = input_parameters[i].format;
    layout.stride = ezRHIResourceFormat::GetFormatStride(layout.format);
    m_locations[input_parameters[i].semantic_name] = input_parameters[i].location;
  }

  for (const auto& entry_point : m_reflection->GetEntryPoints())
  {
    m_ids.emplace(entry_point.name, GenId());
  }
}

ShaderType ShaderBase::GetType() const
{
  return m_shader_type;
}

const ezDynamicArray<ezUInt8>& ShaderBase::GetBlob() const
{
  return m_blob;
}

ezUInt64 ShaderBase::GetId(const ezString& entry_point) const
{
  return m_ids.at(entry_point);
}

const BindKey& ShaderBase::GetBindKey(const ezString& name) const
{
  return m_bind_keys.at(name);
}

const ezDynamicArray<ResourceBindingDesc>& ShaderBase::GetResourceBindings() const
{
  return m_bindings;
}

const ResourceBindingDesc& ShaderBase::GetResourceBinding(const BindKey& bind_key) const
{
  return m_bindings[m_mapping.at(bind_key)];
}

const ezDynamicArray<InputLayoutDesc>& ShaderBase::GetInputLayouts() const
{
  return m_input_layout_descs;
}

ezUInt32 ShaderBase::GetInputLayoutLocation(const ezString& semantic_name) const
{
  return m_locations.at(semantic_name);
}

const ezDynamicArray<BindKey>& ShaderBase::GetBindings() const
{
  return m_binding_keys;
}

const ezSharedPtr<ShaderReflection>& ShaderBase::GetReflection() const
{
  return m_reflection;
}
