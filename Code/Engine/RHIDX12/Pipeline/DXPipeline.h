#pragma once
#include <RHI/Pipeline/Pipeline.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXPipeline : public Pipeline
{
public:
    virtual ~DXPipeline() = default;
    virtual const ComPtr<ID3D12RootSignature>& GetRootSignature() const = 0;
    ezDynamicArray<ezUInt8> GetRayTracingShaderGroupHandles(ezUInt32 first_group, ezUInt32 group_count) const override;
};
