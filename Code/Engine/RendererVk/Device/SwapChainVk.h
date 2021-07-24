
#pragma once

#include <RendererFoundation/Descriptors/Descriptors.h>
#include <RendererFoundation/Device/SwapChain.h>
#include <RendererVk/RendererVkDLL.h>
#include <vulkan/vulkan.hpp>

class ezGALSwapChainVk : public ezGALSwapChain
{
public:
  ezGALResourceFormat::Enum GetFormat() const;
  ezGALTextureHandle GetBackBuffer(ezUInt32 buffer) const;
  ezUInt32 NextImage(const ezGALFence* pFence, ezUInt64 signalValue);
  void Present(const ezGALFence* pFence, ezUInt64 waitValue);

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALSwapChainVk(const ezGALSwapChainCreationDescription& Description, ezInternal::Vk::CommandQueue& commandQueue);

  virtual ~ezGALSwapChainVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;

private:
  ezInternal::Vk::CommandQueue& m_CommandQueue;
  ezGALDeviceVk* m_pDevice;
  vk::UniqueSurfaceKHR m_Surface;
  vk::Format m_SwapchainColorFormat = vk::Format::eB8G8R8Unorm;
  vk::UniqueSwapchainKHR m_Swapchain;
  ezDynamicArray<ezGALTextureHandle> m_BackBuffers;
  ezUInt32 m_FrameIndex = 0;
  vk::UniqueSemaphore m_ImageAvailableSemaphore;
  vk::UniqueSemaphore m_RenderingFinishedSemaphore;
  ezSharedPtr<ezInternal::Vk::CommandList> m_pCommandList;
  ezGALFenceHandle m_hFence;
};

#include <RendererVk/Device/Implementation/SwapChainVk_inl.h>
