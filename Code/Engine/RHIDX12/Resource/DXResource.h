#pragma once
#include <RHI/Resource/ResourceBase.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice;

class DXResource : public ResourceBase
{
public:
    DXResource(DXDevice& device);

    void CommitMemory(MemoryType memory_type) override;
    void BindMemory(const ezSharedPtr<Memory>& memory, ezUInt64 offset) override;
    ezUInt64 GetWidth() const override;
    ezUInt32 GetHeight() const override;
    uint16_t GetLayerCount() const override;
    uint16_t GetLevelCount() const override;
    ezUInt32 GetSampleCount() const override;
    ezUInt64 GetAccelerationStructureHandle() const override;
    void SetName(const ezString& name) override;
    ezUInt8* Map() override;
    void Unmap() override;
    bool AllowCommonStatePromotion(ResourceState state_after) override;
    MemoryRequirements GetMemoryRequirements() const override;

    ComPtr<ID3D12Resource> resource;
    D3D12_RESOURCE_DESC desc = {};
    D3D12_SAMPLER_DESC sampler_desc = {};
    D3D12_GPU_VIRTUAL_ADDRESS acceleration_structure_handle = {};

private:
    DXDevice& m_device;
};
