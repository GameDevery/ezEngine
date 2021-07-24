#pragma once
#include <RHI/Fence/Fence.h>
#include <dxgi.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXFence : public Fence
{
public:
    DXFence(DXDevice& device, ezUInt64 initialValue);
    ezUInt64 GetCompletedValue() override;
    void Wait(ezUInt64 value) override;
    void Signal(ezUInt64 value) override;

    ComPtr<ID3D12Fence> GetFence();

private:
    DXDevice& m_Device;
    ComPtr<ID3D12Fence> m_pFence;
    HANDLE m_hFenceEvent = nullptr;
};
