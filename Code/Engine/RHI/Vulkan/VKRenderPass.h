#pragma once

#include <RHI/Base/RenderPass.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPURenderPass;

class EZ_VULKAN_API CCVKRenderPass final : public RenderPass
{
public:
  CCVKRenderPass();
  ~CCVKRenderPass() override;

  inline CCVKGPURenderPass* gpuRenderPass() const { return _gpuRenderPass; }

protected:
  void doInit(const RenderPassInfo& info) override;
  void doDestroy() override;

  CCVKGPURenderPass* _gpuRenderPass = nullptr;
};
