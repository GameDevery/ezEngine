#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <memory>

class VKGPUBindlessDescriptorPoolTyped;

class VKGPUDescriptorPoolRange
{
public:
  VKGPUDescriptorPoolRange(VKGPUBindlessDescriptorPoolTyped& pool,
    vk::DescriptorSet descriptor_set,
    ezUInt32 offset,
    ezUInt32 size,
    vk::DescriptorType type);
  ~VKGPUDescriptorPoolRange();

  vk::DescriptorSet GetDescriptoSet() const;
  ezUInt32 GetOffset() const;

private:
  std::reference_wrapper<VKGPUBindlessDescriptorPoolTyped> m_pool;
  vk::DescriptorSet m_descriptor_set;
  ezUInt32 m_offset;
  ezUInt32 m_size;
  vk::DescriptorType m_type;
  //std::unique_ptr<VKGPUDescriptorPoolRange, std::function<void(VKGPUDescriptorPoolRange*)>> m_callback;
};
