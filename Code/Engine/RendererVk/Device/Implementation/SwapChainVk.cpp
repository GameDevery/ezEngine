#include <RendererVkPCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Device/SwapChainVk.h>

#include <Foundation/Basics/Platform/Win/HResultUtils.h>
#include <RendererVk/Implementation/Adapter.h>
#include <RendererVk/Implementation/CommandList.h>
#include <RendererVk/Implementation/CommandQueue.h>
#include <RendererVk/Implementation/Instance.h>
#include <RendererVk/Resources/FenceVk.h>

EZ_DEFINE_AS_POD_TYPE(vk::PresentModeKHR);
EZ_DEFINE_AS_POD_TYPE(vk::Image);

ezGALSwapChainVk::ezGALSwapChainVk(const ezGALSwapChainCreationDescription& Description, ezInternal::Vk::CommandQueue& commandQueue)
  : ezGALSwapChain(Description)
  , m_pDevice{nullptr}
  , m_CommandQueue{commandQueue}
{
}

ezGALSwapChainVk::~ezGALSwapChainVk() {}

ezGALResourceFormat::Enum ezGALSwapChainVk::GetFormat() const
{
  //return m_SwapchainColorFormat;
  return ezGALResourceFormat::Invalid;
}

ezGALTextureHandle ezGALSwapChainVk::GetBackBuffer(ezUInt32 buffer) const
{
  return m_BackBuffers[buffer];
}

ezUInt32 ezGALSwapChainVk::NextImage(const ezGALFence* pFence, ezUInt64 signalValue)
{
  vk::Result result = m_pDevice->GetVkDevice().acquireNextImageKHR(m_Swapchain.get(), UINT64_MAX, m_ImageAvailableSemaphore.get(), nullptr, &m_FrameIndex);

  const ezGALFenceVk* vkFence = static_cast<const ezGALFenceVk*>(pFence);

  ezUInt64 tmp = ezMath::MaxValue<ezUInt64>();
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
  signalSubmitInfo.pSignalSemaphores = &vkFence->GetVkFence();
  result = m_CommandQueue.GetQueue().submit(1, &signalSubmitInfo, {});

  return m_FrameIndex;
}

void ezGALSwapChainVk::Present(const ezGALFence* pFence, ezUInt64 waitValue)
{
  const ezGALFenceVk* vkFence = static_cast<const ezGALFenceVk*>(pFence);
  ezUInt64 tmp = ezMath::MaxValue<ezUInt64>();
  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.waitSemaphoreValueCount = 1;
  timelineInfo.pWaitSemaphoreValues = &waitValue;
  timelineInfo.signalSemaphoreValueCount = 1;
  timelineInfo.pSignalSemaphoreValues = &tmp;
  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.waitSemaphoreCount = 1;
  signalSubmitInfo.pWaitSemaphores = &vkFence->GetVkFence();
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

ezResult ezGALSwapChainVk::InitPlatform(ezGALDevice* pDevice)
{
  m_pDevice = static_cast<ezGALDeviceVk*>(pDevice);

  ezInternal::Vk::Instance* instance = m_pDevice->GetVkInstance();

  ezUInt32 width = m_Description.m_pWindow->GetClientAreaSize().width;
  ezUInt32 height = m_Description.m_pWindow->GetClientAreaSize().height;

  // todo: pass these in
  bool vsync = true;
  ezUInt32 frameCount = 2;

  vk::Win32SurfaceCreateInfoKHR surfaceDesc = {};
  surfaceDesc.hinstance = GetModuleHandle(nullptr);
  surfaceDesc.hwnd = reinterpret_cast<HWND>(m_Description.m_pWindow->GetNativeWindowHandle());
  m_Surface = instance->GetInstance().createWin32SurfaceKHRUnique(surfaceDesc);

  auto surfaceFormats = m_pDevice->GetAdapter()->GetPhysicalDevice().getSurfaceFormatsKHR(m_Surface.get());
  EZ_ASSERT_ALWAYS(!surfaceFormats.empty(), "");

  if (surfaceFormats.front().format != vk::Format::eUndefined)
    m_SwapchainColorFormat = surfaceFormats.front().format;

  vk::ColorSpaceKHR colorSpace = surfaceFormats.front().colorSpace;

  vk::SurfaceCapabilitiesKHR surfaceCapabilities = {};
  EZ_ASSERT_ALWAYS(m_pDevice->GetAdapter()->GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface.get(), &surfaceCapabilities) == vk::Result::eSuccess, "");

  EZ_ASSERT_ALWAYS(surfaceCapabilities.currentExtent.width == width, "");
  EZ_ASSERT_ALWAYS(surfaceCapabilities.currentExtent.height == height, "");

  vk::Bool32 isSupportedSurface = VK_FALSE;
  vk::Result result = m_pDevice->GetAdapter()->GetPhysicalDevice().getSurfaceSupportKHR(m_CommandQueue.GetQueueFamilyIndex(), m_Surface.get(), &isSupportedSurface);
  EZ_ASSERT_ALWAYS(isSupportedSurface, "");

  ezUInt32 presentModesCount = 0;
  result = m_pDevice->GetAdapter()->GetPhysicalDevice().getSurfacePresentModesKHR(m_Surface.get(), &presentModesCount, nullptr);

  ezDynamicArray<vk::PresentModeKHR> presentModes;
  presentModes.SetCountUninitialized(presentModesCount);
  result = m_pDevice->GetAdapter()->GetPhysicalDevice().getSurfacePresentModesKHR(m_Surface.get(), &presentModesCount, presentModes.GetData());

  vk::SwapchainCreateInfoKHR swapChainCreateInfo = {};
  swapChainCreateInfo.surface = m_Surface.get();
  swapChainCreateInfo.minImageCount = frameCount;
  swapChainCreateInfo.imageFormat = m_SwapchainColorFormat;
  swapChainCreateInfo.imageColorSpace = colorSpace;
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

  m_Swapchain = m_pDevice->GetVkDevice().createSwapchainKHRUnique(swapChainCreateInfo);

  ezUInt32 imageCount = 0;
  m_pDevice->GetVkDevice().getSwapchainImagesKHR(m_Swapchain.get(), &imageCount, nullptr);

  ezDynamicArray<vk::Image> images;
  images.SetCountUninitialized(imageCount);
  m_pDevice->GetVkDevice().getSwapchainImagesKHR(m_Swapchain.get(), &imageCount, images.GetData());

  m_pCommandList = m_pDevice->CreateCommandList(ezInternal::Vk::CommandListType::Graphics);
  for (ezUInt32 i = 0; i < frameCount; ++i)
  {
    ezGALTextureCreationDescription textureDesc;
    textureDesc.m_Format = GetFormat();
    textureDesc.m_uiWidth = width;
    textureDesc.m_uiHeight = height;
    textureDesc.m_pExisitingNativeObject = images[i];

    ezGALTextureHandle hTexture = m_pDevice->CreateTexture(textureDesc);

    // todo: resource state
    // todo: insert barrier

    //std::shared_ptr<VKResource> res = std::make_shared<VKResource>(m_device);
    //res->format = GetFormat();
    //res->image.res = m_images[i];
    //res->image.format = m_swapchain_color_format;
    //res->image.size = vk::Extent2D(1u * width, 1u * height);
    //res->resource_type = ResourceType::kTexture;
    //res->is_back_buffer = true;
    //m_command_list->ResourceBarrier({{res, ResourceState::kUndefined, ResourceState::kPresent}});
    //res->SetInitialState(ResourceState::kPresent);
    //m_back_buffers.emplace_back(res);
    m_BackBuffers.PushBack(hTexture);
  }
  m_pCommandList->Close();

  vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
  m_ImageAvailableSemaphore = m_pDevice->GetVkDevice().createSemaphoreUnique(semaphoreCreateInfo);
  m_RenderingFinishedSemaphore = m_pDevice->GetVkDevice().createSemaphoreUnique(semaphoreCreateInfo);
  m_hFence = m_pDevice->CreateFence(0);
  m_CommandQueue.ExecuteCommandList(m_pCommandList);
  m_CommandQueue.Signal(m_pDevice->GetFence(m_hFence), 1);

  return EZ_SUCCESS;
}

ezResult ezGALSwapChainVk::DeInitPlatform(ezGALDevice* pDevice)
{
  const ezGALFence* pFence = m_pDevice->GetFence(m_hFence);
  pFence->Wait(1);

  m_pDevice->DestroyFence(m_hFence);

  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_Device_Implementation_SwapChainVk);
