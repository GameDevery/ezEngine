#include <RHI/Program/ProgramBase.h>
#include <deque>

ProgramBase::ProgramBase(const ezDynamicArray<ezSharedPtr<Shader>>& shaders)
    : m_shaders(shaders)
{
    for (const auto& shader : m_shaders)
    {
        m_shaders_by_type[shader->GetType()] = shader;
        decltype(auto) bindings = shader->GetBindings();

        // todo: add InsertRange to ezDynamicArray
        ezDynamicArray<BindKey> tmp(m_bindings);
        m_bindings = bindings;
        m_bindings.PushBackRange(tmp);

        //m_bindings.insert(m_bindings.begin(), bindings.begin(), bindings.end());

        decltype(auto) reflection = shader->GetReflection();
        decltype(auto) shader_entry_points = reflection->GetEntryPoints();
        m_entry_points.PushBackRange(shader_entry_points);
    }
}

bool ProgramBase::HasShader(ShaderType type) const
{
    return m_shaders_by_type.count(type);
}

ezSharedPtr<Shader> ProgramBase::GetShader(ShaderType type) const
{
    auto it = m_shaders_by_type.find(type);
    if (it != m_shaders_by_type.end())
    {
        return it->second;
    }
    return {};
}

const ezDynamicArray<ezSharedPtr<Shader>>& ProgramBase::GetShaders() const
{
    return m_shaders;
}

const ezDynamicArray<BindKey>& ProgramBase::GetBindings() const
{
    return m_bindings;
}

const ezDynamicArray<EntryPoint>& ProgramBase::GetEntryPoints() const
{
    return m_entry_points;
}
