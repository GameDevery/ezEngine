#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Shader/Shader.h>
#include <RHI/Shader/ShaderReflection.h>

class EZ_RHI_DLL ezRHIProgram
{
public:
  ezRHIProgram(const ezDynamicArray<ezSharedPtr<ezRHIShader>>& shaders);

  bool HasShader(ezRHIShaderType type) const;
  ezSharedPtr<ezRHIShader> GetShader(ezRHIShaderType type) const;
  const ezDynamicArray<ezSharedPtr<ezRHIShader>>& GetShaders() const;
  const ezDynamicArray<ezRHIBindKeyDescription>& GetBindings() const;
  const ezDynamicArray<ezRHIEntryPoint>& GetEntryPoints() const;

protected:
  ezMap<ezRHIShaderType, ezSharedPtr<ezRHIShader>> m_ShadersByType;
  ezDynamicArray<ezSharedPtr<ezRHIShader>> m_Shaders;
  ezDynamicArray<ezRHIBindKeyDescription> m_Bindings;
  ezDynamicArray<ezRHIEntryPoint> m_EntryPoints;
};
