#include <RHIVulkanPCH.h>

#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>
#include <RHIVulkan/Instance/VKInstance.h>
#include <RHIVulkan/Resource/VKResource.h>
#include <RHIVulkan/Swapchain/VKSwapchain.h>
#include <RHIVulkan/Utilities/VKUtility.h>

#include <Foundation/Basics/Platform/Win/IncludeWindows.h>

EZ_DEFINE_AS_POD_TYPE(vk::Image);
EZ_DEFINE_AS_POD_TYPE(vk::PresentModeKHR);
EZ_DEFINE_AS_POD_TYPE(vk::SurfaceFormatKHR);

VKSwapchain::VKSwapchain(VKCommandQueue& commandQueue, Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frameCount, bool vsync)
  : m_CommandQueue(commandQueue)
  , m_Device(commandQueue.GetDevice())
{
  VKAdapter& adapter = m_Device.GetAdapter();
  VKInstance& instance = adapter.GetInstance();

  vk::Win32SurfaceCreateInfoKHR surfaceDesc = {};
  surfaceDesc.hinstance = GetModuleHandle(nullptr);
  surfaceDesc.hwnd = reinterpret_cast<HWND>(window);
  m_Surface = instance.GetInstance().createWin32SurfaceKHRUnique(surfaceDesc);

  ezUInt32 surfaceFormatsCount = 0;
  vk::Result result = adapter.GetPhysicalDevice().getSurfaceFormatsKHR(m_Surface.get(), &surfaceFormatsCount, nullptr);

  ezDynamicArray<vk::SurfaceFormatKHR> surfaceFormats;
  surfaceFormats.SetCountUninitialized(surfaceFormatsCount);
  result = adapter.GetPhysicalDevice().getSurfaceFormatsKHR(m_Surface.get(), &surfaceFormatsCount, surfaceFormats.GetData());

  EZ_ASSERT_ALWAYS(!surfaceFormats.IsEmpty(), "");

  if (surfaceFormats[0].format != vk::Format::eUndefined)
    m_SwapchainColorFormat = surfaceFormats[0].format;

  vk::ColorSpaceKHR color_space = surfaceFormats[0].colorSpace;

  vk::SurfaceCapabilitiesKHR surfaceCapabilities = {};
  EZ_ASSERT_ALWAYS(adapter.GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface.get(), &surfaceCapabilities) == vk::Result::eSuccess, "");

  EZ_ASSERT_ALWAYS(surfaceCapabilities.currentExtent.width == width, "");
  EZ_ASSERT_ALWAYS(surfaceCapabilities.currentExtent.height == height, "");

  vk::Bool32 isSupportedSurface = VK_FALSE;
  result = adapter.GetPhysicalDevice().getSurfaceSupportKHR(commandQueue.GetQueueFamilyIndex(), m_Surface.get(), &isSupportedSurface);
  EZ_ASSERT_ALWAYS(isSupportedSurface, "");

  ezUInt32 presentModesCount = 0;
  result = adapter.GetPhysicalDevice().getSurfacePresentModesKHR(m_Surface.get(), &presentModesCount, nullptr);

  ezDynamicArray<vk::PresentModeKHR> presentModes;
  presentModes.SetCountUninitialized(presentModesCount);
  result = adapter.GetPhysicalDevice().getSurfacePresentModesKHR(m_Surface.get(), &presentModesCount, presentModes.GetData());

  vk::SwapchainCreateInfoKHR swapChainCreateInfo = {};
  swapChainCreateInfo.surface = m_Surface.get();
  swapChainCreateInfo.minImageCount = frameCount;
  swapChainCreateInfo.imageFormat = m_SwapchainColorFormat;
  swapChainCreateInfo.imageColorSpace = color_space;
  swapChainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
  swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
  swapChainCreateInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
  swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  if (vsync)
  {
    if (presentModes.Contains(vk::PresentModeKHR::eFifoRelaxed))
      swapChainCreateInfo.presentMode = vk::PresentModeKHR::eFifoRelaxed;
    else
      swapChainCreateInfo.presentMode = vk::PresentModeKHR::eFifo;
  }
  else
  {
    if (presentModes.Contains(vk::PresentModeKHR::eMailbox))
      swapChainCreateInfo.presentMode = vk::PresentModeKHR::eMailbox;
    else
      swapChainCreateInfo.presentMode = vk::PresentModeKHR::eImmediate;
  }
  swapChainCreateInfo.clipped = true;

  m_Swapchain = m_Device.GetDevice().createSwapchainKHRUnique(swapChainCreateInfo);

  ezUInt32 imageCount = 0;
  result = m_Device.GetDevice().getSwapchainImagesKHR(m_Swapchain.get(), &imageCount, nullptr);

  ezDynamicArray<vk::Image> images;
  images.SetCountUninitialized(imageCount);
  result = m_Device.GetDevice().getSwapchainImagesKHR(m_Swapchain.get(), &imageCount, images.GetData());

  m_CommandList = m_Device.CreateCommandList(CommandListType::kGraphics);
  for (ezUInt32 i = 0; i < frameCount; ++i)
  {
    std::shared_ptr<VKResource> res = std::make_shared<VKResource>(m_Device);
    res->format = GetFormat();
    res->image.res = images[i];
    res->image.format = m_SwapchainColorFormat;
    res->image.size = vk::Extent2D(1u * width, 1u * height);
    res->resource_type = ResourceType::kTexture;
    res->is_back_buffer = true;
    m_CommandList->ResourceBarrier({{res, ResourceState::kUndefined, ResourceState::kPresent}});
    res->SetInitialState(ResourceState::kPresent);
    m_BackBuffers.PushBack(res);
  }
  m_CommandList->Close();

  vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
  m_ImageAvailableSemaphore = m_Device.GetDevice().createSemaphoreUnique(semaphoreCreateInfo);
  m_RenderingFinishedSemaphore = m_Device.GetDevice().createSemaphoreUnique(semaphoreCreateInfo);
  m_Fence = m_Device.CreateFence(0);
  commandQueue.ExecuteCommandLists({m_CommandList});
  commandQueue.Signal(m_Fence, 1);
}

VKSwapchain::~VKSwapchain()
{
  m_Fence->Wait(1);
}

ezRHIResourceFormat::Enum VKSwapchain::GetFormat() const
{
  return VKUtils::ToEngineFormat(m_SwapchainColorFormat);
}

std::shared_ptr<Resource> VKSwapchain::GetBackBuffer(ezUInt32 buffer)
{
  return m_BackBuffers[buffer];
}

ezUInt32 VKSwapchain::NextImage(const std::shared_ptr<Fence>& fence, ezUInt64 signalValue)
{
  vk::Result result = m_Device.GetDevice().acquireNextImageKHR(m_Swapchain.get(), UINT64_MAX, m_ImageAvailableSemaphore.get(), nullptr, &m_FrameIndex);

  VKTimelineSemaphore& vkFence = fence->As<VKTimelineSemaphore>();
  constexpr ezUInt64 tmp = ezMath::MaxValue<ezUInt64>();
  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.waitSemaphoreValueCount = 1;
  timelineInfo.pWaitSemaphoreValues = &tmp;
  timelineInfo.signalSemaphoreValueCount = 1;
  timelineInfo.pSignalSemaphoreValues = &signalValue;
  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.waitSemaphoreCount = 1;
  signalSubmitInfo.pWaitSemaphores = &m_ImageAvailableSemaphore.get();
  vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eTransfer;
  signalSubmitInfo.pWaitDstStageMask = &waitDstStageMask;
  signalSubmitInfo.signalSemaphoreCount = 1;
  signalSubmitInfo.pSignalSemaphores = &vkFence.GetFence();
  result = m_CommandQueue.GetQueue().submit(1, &signalSubmitInfo, {});

  return m_FrameIndex;
}

void VKSwapchain::Present(const std::shared_ptr<Fence>& fence, ezUInt64 waitValue)
{
  decltype(auto) vkFence = fence->As<VKTimelineSemaphore>();
  constexpr ezUInt64 tmp = ezMath::MaxValue<ezUInt64>();
  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.waitSemaphoreValueCount = 1;
  timelineInfo.pWaitSemaphoreValues = &waitValue;
  timelineInfo.signalSemaphoreValueCount = 1;
  timelineInfo.pSignalSemaphoreValues = &tmp;
  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.waitSemaphoreCount = 1;
  signalSubmitInfo.pWaitSemaphores = &vkFence.GetFence();
  vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eTransfer;
  signalSubmitInfo.pWaitDstStageMask = &waitDstStageMask;
  signalSubmitInfo.signalSemaphoreCount = 1;
  signalSubmitInfo.pSignalSemaphores = &m_RenderingFinishedSemaphore.get();
  vk::Result result = m_CommandQueue.GetQueue().submit(1, &signalSubmitInfo, {});

  vk::PresentInfoKHR presentInfo = {};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &m_Swapchain.get();
  presentInfo.pImageIndices = &m_FrameIndex;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &m_RenderingFinishedSemaphore.get();
  result = m_CommandQueue.GetQueue().presentKHR(presentInfo);
}
