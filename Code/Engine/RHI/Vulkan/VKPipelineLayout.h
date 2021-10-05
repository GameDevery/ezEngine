#pragma once

#include <RHI/Base/PipelineLayout.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUPipelineLayout;

class EZ_VULKAN_API CCVKPipelineLayout final : public PipelineLayout
{
public:
  CCVKPipelineLayout();
  ~CCVKPipelineLayout() override;

  inline CCVKGPUPipelineLayout* gpuPipelineLayout() const { return _gpuPipelineLayout; }

protected:
  void doInit(const PipelineLayoutInfo& info) override;
  void doDestroy() override;

  CCVKGPUPipelineLayout* _gpuPipelineLayout = nullptr;
};
