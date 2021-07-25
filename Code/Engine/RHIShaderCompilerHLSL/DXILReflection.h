#pragma once
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <RHIShaderCompilerHLSL/DXCLoader.h>
#include <d3d12shader.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXILReflection : public ShaderReflection
{
public:
  DXILReflection(const void* data, size_t size);
  ~DXILReflection();
  const ezDynamicArray<EntryPoint>& GetEntryPoints() const override;
  const ezDynamicArray<ResourceBindingDesc>& GetBindings() const override;
  const ezDynamicArray<VariableLayout>& GetVariableLayouts() const override;
  const ezDynamicArray<InputParameterDesc>& GetInputParameters() const override;
  const ezDynamicArray<OutputParameterDesc>& GetOutputParameters() const override;
  const ShaderFeatureInfo& GetShaderFeatureInfo() const override;

private:
  void ParseRuntimeData(ComPtr<IDxcContainerReflection> reflection, uint32_t idx);
  void ParseShaderReflection(ComPtr<ID3D12ShaderReflection> shader_reflection);
  void ParseLibraryReflection(ComPtr<ID3D12LibraryReflection> library_reflection);
  void ParseDebugInfo(dxc::DxcDllSupport& dxc_support, ComPtr<IDxcBlob> pdb);

  bool m_is_library = false;
  ezDynamicArray<EntryPoint> m_entry_points;
  ezDynamicArray<ResourceBindingDesc> m_bindings;
  ezDynamicArray<VariableLayout> m_layouts;
  ezDynamicArray<InputParameterDesc> m_input_parameters;
  ezDynamicArray<OutputParameterDesc> m_output_parameters;
  ShaderFeatureInfo m_shader_feature_info = {};
};
