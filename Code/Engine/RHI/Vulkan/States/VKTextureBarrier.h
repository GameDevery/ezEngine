#pragma once

#include <RHI/Base/States/TextureBarrier.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUTextureBarrier;

class EZ_VULKAN_API CCVKTextureBarrier : public TextureBarrier
{
public:
  explicit CCVKTextureBarrier(const TextureBarrierInfo& info, ezUInt32 hash);
  ~CCVKTextureBarrier() override;

  inline const CCVKGPUTextureBarrier* gpuBarrier() const { return _gpuBarrier; }

protected:
  CCVKGPUTextureBarrier* _gpuBarrier = nullptr;
};
