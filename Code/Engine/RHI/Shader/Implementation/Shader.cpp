#include <RHIPCH.h>

#include <RHI/Shader/Shader.h>
#include <RHI/Shader/ShaderReflection.h>

static ezUInt64 GenId()
{
  static ezUInt64 id = 0;
  return ++id;
}

ezRHIShader::ezRHIShader(const ezRHIShaderCreationDescription& desc)
{
  //m_Blob = Compile(desc, blob_type);
  //m_Reflection = CreateShaderReflection(blob_type, m_Blob.GetData(), m_Blob.GetSize());
  m_Bindings = m_Reflection->GetBindings();
  for (ezUInt32 i = 0; i < m_Bindings.GetCount(); ++i)
  {
    ezRHIBindKeyDescription bindKey;
    {
      bindKey.m_ShaderType = m_ShaderType;
      bindKey.m_ViewType = m_Bindings[i].m_Type;
      bindKey.m_Slot = m_Bindings[i].m_Slot;
      bindKey.m_Space = m_Bindings[i].m_Space;
      bindKey.m_Count = m_Bindings[i].m_Count;
    }

    m_BindKeys[m_Bindings[i].m_Name] = bindKey;
    m_Mapping[bindKey] = i;
    m_BindingKeys.PushBack(bindKey);
  }

  const ezDynamicArray<ezRHIInputParameterDescription>& inputParameters = m_Reflection->GetInputParameters();
  for (ezUInt32 i = 0; i < inputParameters.GetCount(); ++i)
  {
    auto& layout = m_InputLayoutDescs.ExpandAndGetRef();
    layout.m_Slot = i;
    layout.m_SemanticName = inputParameters[i].m_SemanticName;
    layout.m_Format = inputParameters[i].m_Format;
    layout.m_Stride = GetFormatInfo(layout.m_Format).m_Stride;
    m_Locations[inputParameters[i].m_SemanticName] = inputParameters[i].m_Location;
  }

  for (const auto& entryPoint : m_Reflection->GetEntryPoints())
  {
    m_Ids.Insert(entryPoint.m_Name, GenId());
  }
}

ezRHIShaderType ezRHIShader::GetType() const
{
  return m_ShaderType;
}

const ezDynamicArray<ezUInt8>& ezRHIShader::GetBlob() const
{
  return m_Blob;
}

ezUInt64 ezRHIShader::GetId(const ezString& entryPoint) const
{
  return *m_Ids.GetValue(entryPoint);
}

const ezRHIBindKeyDescription& ezRHIShader::GetBindKey(const ezString& name) const
{
  return *m_BindKeys.GetValue(name);
}

const ezDynamicArray<ezRHIResourceBindingDescription>& ezRHIShader::GetResourceBindings() const
{
  return m_Bindings;
}

const ezRHIResourceBindingDescription& ezRHIShader::GetResourceBinding(const ezRHIBindKeyDescription& bindKey) const
{
  return m_Bindings[*m_Mapping.GetValue(bindKey)];
}

const ezDynamicArray<ezRHIInputElementDescription>& ezRHIShader::GetInputLayouts() const
{
  return m_InputLayoutDescs;
}

ezUInt32 ezRHIShader::GetInputLayoutLocation(const ezString& semanticName) const
{
  return *m_Locations.GetValue(semanticName);
}

const ezDynamicArray<ezRHIBindKeyDescription>& ezRHIShader::GetBindings() const
{
  return m_BindingKeys;
}

const ezUniquePtr<ezRHIShaderReflection>& ezRHIShader::GetReflection() const
{
  return m_Reflection;
}
