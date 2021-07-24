#include <RHIDX12/BindingSet/DXBindingSet.h>
#include <RHIDX12/BindingSetLayout/DXBindingSetLayout.h>
#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPoolRange.h>
#include <RHIDX12/Program/DXProgram.h>
#include <RHIDX12/View/DXView.h>

DXBindingSet::DXBindingSet(DXDevice& device, const ezSharedPtr<DXBindingSetLayout>& layout)
  : m_Device(device)
  , m_Layout(layout)
{
  for (const auto& desc : m_Layout->GetHeapDescs())
  {
    ezSharedPtr<DXGPUDescriptorPoolRange> heapRange = EZ_DEFAULT_NEW(DXGPUDescriptorPoolRange, m_Device.GetGPUDescriptorPool().Allocate(desc.Key(), desc.Value()));
    m_DescriptorRanges.Insert(desc.Key(), heapRange);
  }
}

void DXBindingSet::WriteBindings(const std::vector<BindingDesc>& bindings)
{
  for (const auto& binding : bindings)
  {
    if (!binding.view)
    {
      continue;
    }
    const BindingLayout& bindingLayout = *m_Layout->GetLayout().GetValue(binding.bind_key);
    ezSharedPtr<DXGPUDescriptorPoolRange> heapRange = m_DescriptorRanges[bindingLayout.heap_type];
    decltype(auto) srcCPUHandle = binding.view.Downcast<DXView>()->GetHandle();
    heapRange->CopyCpuHandle(bindingLayout.heap_offset, srcCPUHandle);
  }
}

void SetRootDescriptorTable(const ComPtr<ID3D12GraphicsCommandList>& commandList, ezUInt32 rootParameter, bool isCompute, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor)
{
  if (isCompute)
  {
    commandList->SetComputeRootDescriptorTable(rootParameter, baseDescriptor);
  }
  else
  {
    commandList->SetGraphicsRootDescriptorTable(rootParameter, baseDescriptor);
  }
}

ezDynamicArray<ComPtr<ID3D12DescriptorHeap>> DXBindingSet::Apply(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
  ezMap<D3D12_DESCRIPTOR_HEAP_TYPE, ComPtr<ID3D12DescriptorHeap>> heapMap;
  for (const auto& table : m_Layout->GetDescriptorTables())
  {
    D3D12_DESCRIPTOR_HEAP_TYPE heapType = table.Value().heap_type;
    ComPtr<ID3D12DescriptorHeap> heap;
    if (table.Value().bindless)
    {
      heap = m_Device.GetGPUDescriptorPool().GetHeap(heapType);
    }
    else
    {
      heap = m_DescriptorRanges[heapType]->GetHeap();
    }

    auto it = heapMap.Find(heapType);
    if (it == end(heapMap))
    {
      heapMap.Insert(heapType, m_Device.GetGPUDescriptorPool().GetHeap(heapType));
    }
    else
    {
      EZ_ASSERT_ALWAYS(it.Value() == heap, "");
    }
  }

  ezDynamicArray<ComPtr<ID3D12DescriptorHeap>> descriptorHeaps;
  ezDynamicArray<ID3D12DescriptorHeap*> descriptorHeapsPtr;
  for (const auto& heap : heapMap)
  {
    descriptorHeaps.PushBack(heap.Value());
    descriptorHeapsPtr.PushBack(heap.Value().Get());
  }
  if (descriptorHeapsPtr.GetCount())
  {
    commandList->SetDescriptorHeaps(descriptorHeapsPtr.GetCount(), descriptorHeapsPtr.GetData());
  }

  for (const auto& table : m_Layout->GetDescriptorTables())
  {
    D3D12_DESCRIPTOR_HEAP_TYPE heap_type = table.Value().heap_type;
    D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor = {};
    if (table.Value().bindless)
    {
      baseDescriptor = m_Device.GetGPUDescriptorPool().GetHeap(heap_type)->GetGPUDescriptorHandleForHeapStart();
    }
    else
    {
      decltype(auto) heapRange = m_DescriptorRanges[heap_type];
      baseDescriptor = heapRange->GetGpuHandle(table.Value().heap_offset);
    }
    SetRootDescriptorTable(commandList, table.Key(), table.Value().is_compute, baseDescriptor);
  }
  return descriptorHeaps;
}
