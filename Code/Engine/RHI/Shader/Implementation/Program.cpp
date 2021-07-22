#include <RHIPCH.h>

#include <RHI/Shader/Program.h>

ezRHIProgram::ezRHIProgram(const ezDynamicArray<ezSharedPtr<ezRHIShader>>& shaders)
  : m_Shaders{shaders}
{
  for (const auto& shader : m_Shaders)
  {
    m_ShadersByType[shader->GetType()] = shader;
    const ezDynamicArray<ezRHIBindKeyDescription>& bindings = shader->GetBindings();
    m_Bindings.PushBackRange(bindings); // todo: insert at front
    //.insert(m_bindings.begin(), bindings.begin(), bindings.end());

    auto& reflection = shader->GetReflection();
    const ezDynamicArray<ezRHIEntryPoint>& shaderEntryPoints = reflection->GetEntryPoints();
    m_EntryPoints.PushBackRange(shaderEntryPoints);
  }
}

bool ezRHIProgram::HasShader(ezRHIShaderType type) const
{
  return m_ShadersByType.Contains(type);
}

ezSharedPtr<ezRHIShader> ezRHIProgram::GetShader(ezRHIShaderType type) const
{
  return ezSharedPtr<ezRHIShader>();
}

const ezDynamicArray<ezSharedPtr<ezRHIShader>>& ezRHIProgram::GetShaders() const
{
  return m_Shaders;
}

const ezDynamicArray<ezRHIBindKeyDescription>& ezRHIProgram::GetBindings() const
{
  return m_Bindings;
}

const ezDynamicArray<ezRHIEntryPoint>& ezRHIProgram::GetEntryPoints() const
{
  return m_EntryPoints;
}
