#pragma once
#include <RHI/Swapchain/Swapchain.h>
#include <RHI/Resource/Resource.h>
#include <cstdint>
#include <vector>
#include <dxgi1_4.h>
#include <directx/d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXCommandQueue;

class DXSwapchain
    : public Swapchain
{
public:
    DXSwapchain(DXCommandQueue& command_queue, Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frame_count, bool vsync);
    ezRHIResourceFormat::Enum GetFormat() const override;
    std::shared_ptr<Resource> GetBackBuffer(ezUInt32 buffer) override;
    ezUInt32 NextImage(const std::shared_ptr<Fence>& fence, ezUInt64 signal_value) override;
    void Present(const std::shared_ptr<Fence>& fence, ezUInt64 wait_value) override;

private:
    DXCommandQueue& m_command_queue;
    bool m_vsync;
    ComPtr<IDXGISwapChain3> m_swap_chain;
    ezDynamicArray<std::shared_ptr<Resource>> m_BackBuffers;
};
