#pragma once
#include <RHI/Resource/Resource.h>
#include <RHI/Swapchain/Swapchain.h>
#include <cstdint>
#include <directx/d3d12.h>
#include <dxgi1_4.h>
#include <vector>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXCommandQueue;

class DXSwapchain : public Swapchain
{
public:
  DXSwapchain(DXCommandQueue& command_queue, Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frameCount, bool vsync);
  ezRHIResourceFormat::Enum GetFormat() const override;
  ezSharedPtr<Resource> GetBackBuffer(ezUInt32 buffer) override;
  ezUInt32 NextImage(const ezSharedPtr<Fence>& fence, ezUInt64 signal_value) override;
  void Present(const ezSharedPtr<Fence>& fence, ezUInt64 wait_value) override;

private:
  DXCommandQueue& m_CommandQueue;
  bool m_vsync;
  ComPtr<IDXGISwapChain3> m_SwapChain;
  ezDynamicArray<ezSharedPtr<Resource>> m_BackBuffers;
};
