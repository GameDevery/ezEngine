#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/ShaderReflection/ShaderReflection.h>
#include <RHIVulkan/Pipeline/VKPipeline.h>
#include <RHIVulkan/RenderPass/VKRenderPass.h>

class VKDevice;

class VKRayTracingPipeline : public VKPipeline
{
public:
  VKRayTracingPipeline(VKDevice& device, const RayTracingPipelineDesc& desc);
  PipelineType GetPipelineType() const override;
  std::vector<ezUInt8> GetRayTracingShaderGroupHandles(ezUInt32 first_group, ezUInt32 group_count) const override;

private:
  RayTracingPipelineDesc m_desc;
};
