#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>
#include <Foundation/Containers/ArrayMap.h>
#include <algorithm>

constexpr ezUInt32 max_bindless_heap_size = 10000;

class VKDevice;

class VKGPUBindlessDescriptorPoolTyped
{
public:
  VKGPUBindlessDescriptorPoolTyped(VKDevice& device, vk::DescriptorType type);
  VKGPUDescriptorPoolRange Allocate(ezUInt32 count);
  void OnRangeDestroy(ezUInt32 offset, ezUInt32 size);
  vk::DescriptorSet GetDescriptorSet() const;

private:
  void ResizeHeap(ezUInt32 req_size);

  VKDevice& m_device;
  vk::DescriptorType m_type;
  ezUInt32 m_size = 0;
  ezUInt32 m_offset = 0;
  struct Descriptor
  {
    vk::UniqueDescriptorPool pool;
    vk::UniqueDescriptorSetLayout set_layout;
    vk::UniqueDescriptorSet set;
  } m_descriptor;
  ezArrayMap<ezUInt32, ezUInt32> m_empty_ranges;
};
