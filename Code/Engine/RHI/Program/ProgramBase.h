#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Program/Program.h>
#include <map>
#include <set>
#include <vector>

class EZ_RHI_DLL ProgramBase : public Program
{
public:
  ProgramBase(const ezDynamicArray<ezSharedPtr<Shader>>& shaders);

  bool HasShader(ShaderType type) const override final;
  ezSharedPtr<Shader> GetShader(ShaderType type) const override final;
  const ezDynamicArray<ezSharedPtr<Shader>>& GetShaders() const override final;
  const ezDynamicArray<BindKey>& GetBindings() const override final;
  const ezDynamicArray<EntryPoint>& GetEntryPoints() const override final;

protected:
  std::map<ShaderType, ezSharedPtr<Shader>> m_shaders_by_type;
  ezDynamicArray<ezSharedPtr<Shader>> m_shaders;
  ezDynamicArray<BindKey> m_bindings;
  ezDynamicArray<EntryPoint> m_entry_points;
};
