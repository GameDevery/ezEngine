#include <RHIDX12/Device/DXDevice.h>
#include <RHIDX12/Fence/DXFence.h>
#include <RHIDX12/Utilities/DXUtility.h>
#include <dxgi1_6.h>
#include <directx/d3d12.h>

DXFence::DXFence(DXDevice& device, ezUInt64 initialValue)
  : m_Device(device)
{
  EZ_ASSERT_ALWAYS(device.GetDevice()->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)) == S_OK, "");
  m_hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

ezUInt64 DXFence::GetCompletedValue()
{
  return m_pFence->GetCompletedValue();
}

void DXFence::Wait(ezUInt64 value)
{
  if (GetCompletedValue() < value)
  {
    EZ_ASSERT_ALWAYS(m_pFence->SetEventOnCompletion(value, m_hFenceEvent) == S_OK, "");
    WaitForSingleObjectEx(m_hFenceEvent, INFINITE, FALSE);
  }
}

void DXFence::Signal(ezUInt64 value)
{
  m_pFence->Signal(value);
}

ComPtr<ID3D12Fence> DXFence::GetFence()
{
  return m_pFence;
}
