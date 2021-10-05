#pragma once

#include <RHI/Base/DescriptorSetLayout.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUDescriptorSetLayout;

class EZ_VULKAN_API CCVKDescriptorSetLayout final : public DescriptorSetLayout
{
public:
  CCVKDescriptorSetLayout();
  ~CCVKDescriptorSetLayout() override;

  inline CCVKGPUDescriptorSetLayout* gpuDescriptorSetLayout() const { return _gpuDescriptorSetLayout; }

protected:
  static ezUInt32 generateID() noexcept
  {
    static ezUInt32 idGen = 10000;
    return idGen++;
  }

  void doInit(const DescriptorSetLayoutInfo& info) override;
  void doDestroy() override;

  CCVKGPUDescriptorSetLayout* _gpuDescriptorSetLayout = nullptr;
};
