#pragma once

#include <RHI/Base/PipelineState.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUPipelineState;

class EZ_VULKAN_API CCVKPipelineState final : public PipelineState
{
public:
  CCVKPipelineState();
  ~CCVKPipelineState() override;

  inline CCVKGPUPipelineState* gpuPipelineState() const { return _gpuPipelineState; }

protected:
  void doInit(const PipelineStateInfo& info) override;
  void doDestroy() override;

  CCVKGPUPipelineState* _gpuPipelineState = nullptr;
};
