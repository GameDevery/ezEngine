#pragma once
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <spirv_hlsl.hpp>
#include <string>
#include <vector>

class SPIRVReflection : public ShaderReflection
{
public:
  SPIRVReflection(const void* data, size_t size);
  const ezDynamicArray<EntryPoint>& GetEntryPoints() const override;
  const ezDynamicArray<ResourceBindingDesc>& GetBindings() const override;
  const ezDynamicArray<VariableLayout>& GetVariableLayouts() const override;
  const ezDynamicArray<InputParameterDesc>& GetInputParameters() const override;
  const ezDynamicArray<OutputParameterDesc>& GetOutputParameters() const override;
  const ShaderFeatureInfo& GetShaderFeatureInfo() const override;

private:
  std::vector<uint32_t> m_blob;
  ezDynamicArray<EntryPoint> m_entry_points;
  ezDynamicArray<ResourceBindingDesc> m_bindings;
  ezDynamicArray<VariableLayout> m_layouts;
  ezDynamicArray<InputParameterDesc> m_input_parameters;
  ezDynamicArray<OutputParameterDesc> m_output_parameters;
  ShaderFeatureInfo m_shader_feature_info = {};
};
