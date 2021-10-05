#pragma once

#include <RHI/Base/DescriptorSet.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUDescriptorSet;

class EZ_VULKAN_API CCVKDescriptorSet final : public DescriptorSet
{
public:
  CCVKDescriptorSet();
  ~CCVKDescriptorSet() override;

  void update() override;

  inline CCVKGPUDescriptorSet* gpuDescriptorSet() const { return _gpuDescriptorSet; }

protected:
  void doInit(const DescriptorSetInfo& info) override;
  void doDestroy() override;

  CCVKGPUDescriptorSet* _gpuDescriptorSet = nullptr;
};
