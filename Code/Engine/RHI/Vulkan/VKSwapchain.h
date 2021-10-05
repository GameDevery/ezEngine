#pragma once

#include <RHI/Base/Swapchain.h>
#include <RHI/Vulkan/VKStd.h>


class CCVKDevice;
class CCVKGPUDevice;
class CCVKGPUSwapchain;
class CCVKGPUSwapchain;
class CCVKGPUSemaphorePool;

class EZ_VULKAN_API CCVKSwapchain final : public Swapchain
{
public:
  static constexpr bool ENABLE_PRE_ROTATION = true;

  CCVKSwapchain();
  ~CCVKSwapchain() override;

  inline CCVKGPUSwapchain* gpuSwapchain() { return _gpuSwapchain; }

  bool checkSwapchainStatus(ezUInt32 width = 0, ezUInt32 height = 0);

protected:
  void doInit(const SwapchainInfo& info) override;
  void doDestroy() override;
  void doResize(ezUInt32 width, ezUInt32 height, SurfaceTransform transform) override;
  void doDestroySurface() override;
  void doCreateSurface(void* windowHandle) override;

  void createVkSurface();
  void destroySwapchain(const CCVKGPUDevice* gpuDevice);

  CCVKGPUSwapchain* _gpuSwapchain = nullptr;
};
