#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Swapchain/Swapchain.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <memory>
#include <vector>

class VKDevice;
class VKCommandQueue;
class CommandList;
class Fence;

class VKSwapchain : public Swapchain
{
public:
  VKSwapchain(VKCommandQueue& commandQueue, Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frameCount, bool vsync);
  ~VKSwapchain();
  ezRHIResourceFormat::Enum GetFormat() const override;
  ezSharedPtr<Resource> GetBackBuffer(ezUInt32 buffer) override;
  ezUInt32 NextImage(const ezSharedPtr<Fence>& fence, ezUInt64 signalValue) override;
  void Present(const ezSharedPtr<Fence>& fence, ezUInt64 waitValue) override;

private:
  VKCommandQueue& m_CommandQueue;
  VKDevice& m_Device;
  vk::UniqueSurfaceKHR m_Surface;
  vk::Format m_SwapchainColorFormat = vk::Format::eB8G8R8Unorm;
  vk::UniqueSwapchainKHR m_Swapchain;
  ezDynamicArray<ezSharedPtr<Resource>> m_BackBuffers;
  ezUInt32 m_FrameIndex = 0;
  vk::UniqueSemaphore m_ImageAvailableSemaphore;
  vk::UniqueSemaphore m_RenderingFinishedSemaphore;
  ezSharedPtr<CommandList> m_CommandList;
  ezSharedPtr<Fence> m_Fence;
};
