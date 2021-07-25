#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/BindingSetLayout/BindingSetLayout.h>

class VKDevice;

class VKBindingSetLayout
  : public BindingSetLayout
{
public:
  VKBindingSetLayout(VKDevice& device, const ezDynamicArray<BindKey>& descs);

  const ezMap<ezUInt32, vk::DescriptorType>& GetBindlessType() const;
  const ezDynamicArray<vk::UniqueDescriptorSetLayout>& GetDescriptorSetLayouts() const;
  const ezDynamicArray<ezMap<vk::DescriptorType, ezUInt32>>& GetDescriptorCountBySet() const;
  vk::PipelineLayout GetPipelineLayout() const;

private:
  ezMap<ezUInt32, vk::DescriptorType> m_bindless_type;
  ezDynamicArray<vk::UniqueDescriptorSetLayout> m_descriptor_set_layouts;
  ezDynamicArray<ezMap<vk::DescriptorType, ezUInt32>> m_descriptor_count_by_set;
  vk::UniquePipelineLayout m_pipeline_layout;
};

vk::DescriptorType GetDescriptorType(ViewType view_type);
