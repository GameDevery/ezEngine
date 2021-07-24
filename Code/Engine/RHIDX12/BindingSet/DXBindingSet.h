#pragma once
#include <RHI/BindingSet/BindingSet.h>
#include <RHIDX12/Program/DXProgram.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice;
class DXBindingSetLayout;
class DXGPUDescriptorPoolRange;

class DXBindingSet
  : public BindingSet
{
public:
  DXBindingSet(DXDevice& device, const ezSharedPtr<DXBindingSetLayout>& layout);

  void WriteBindings(const std::vector<BindingDesc>& bindings) override;

  ezDynamicArray<ComPtr<ID3D12DescriptorHeap>> Apply(const ComPtr<ID3D12GraphicsCommandList>& commandList);

private:
  DXDevice& m_Device;
  ezSharedPtr<DXBindingSetLayout> m_Layout;
  ezMap<D3D12_DESCRIPTOR_HEAP_TYPE, ezSharedPtr<DXGPUDescriptorPoolRange>> m_DescriptorRanges;
};
