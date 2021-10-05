#pragma once

#include <RHI/Base/Framebuffer.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUFramebuffer;

class EZ_VULKAN_API CCVKFramebuffer final : public Framebuffer
{
public:
  CCVKFramebuffer();
  ~CCVKFramebuffer() override;

  inline CCVKGPUFramebuffer* gpuFBO() const { return _gpuFBO; }

protected:
  void doInit(const FramebufferInfo& info) override;
  void doDestroy() override;

  CCVKGPUFramebuffer* _gpuFBO = nullptr;
};
