#pragma once

#include <RHI/Base/States/Sampler.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUSampler;

class EZ_VULKAN_API CCVKSampler final : public Sampler
{
public:
  explicit CCVKSampler(const SamplerInfo& info, ezUInt32 hash);
  ~CCVKSampler() override;

  inline CCVKGPUSampler* gpuSampler() const { return _gpuSampler; }

protected:
  CCVKGPUSampler* _gpuSampler = nullptr;
};
