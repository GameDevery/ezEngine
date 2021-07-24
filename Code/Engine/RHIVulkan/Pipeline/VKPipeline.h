#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Pipeline/Pipeline.h>
#include <RHI/Program/Program.h>
#include <deque>

class VKDevice;

class VKPipeline : public Pipeline
{
public:
  VKPipeline(VKDevice& device, const ezSharedPtr<Program>& program, const ezSharedPtr<BindingSetLayout>& layout);
  vk::PipelineLayout GetPipelineLayout() const;
  vk::Pipeline GetPipeline() const;
  ezDynamicArray<ezUInt8> GetRayTracingShaderGroupHandles(ezUInt32 first_group, ezUInt32 group_count) const override;

protected:
  VKDevice& m_device;
  std::deque<ezString> entry_point_names;
  std::vector<vk::PipelineShaderStageCreateInfo> m_shader_stage_create_info;
  std::vector<vk::UniqueShaderModule> m_shader_modules;
  vk::UniquePipeline m_pipeline;
  vk::PipelineLayout m_pipeline_layout;
  ezMap<ezUInt64, ezUInt32> m_shader_ids;
};
